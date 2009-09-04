#
#  rhom_object_factory.rb
#  rhodes
#  Returns an array of RhomObjects
#
#  Copyright (C) 2008 Rhomobile, Inc. All rights reserved.
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
require 'rhom'
require 'rho'
require 'rho/rhosupport'

module Rhom
  class RhomAttribManager
    @@attribs_map = Hash.new
    @@mxMap = Mutex.new
        
    def RhomAttribManager.reset_all
        @@mxMap.lock
        begin
            @@attribs_map = Hash.new
            ::Rhom::RhomDbAdapter::update_into_table('sources', {"source_attribs"=>""})
            
        ensure    
            @@mxMap.unlock
        end     
    end
    
    def RhomAttribManager.get_attribs(srcid_a)
        @@mxMap.lock
        begin
          #puts '@@attribs_map' + @@attribs_map.to_s
          srcid = srcid_a.to_i          
          attribs = @@attribs_map[srcid]
          return attribs.keys if attribs
          
          []
        ensure    
            @@mxMap.unlock
        end     
          
    end

    def RhomAttribManager._add_attrib(srcid,attr)
        unless ::Rhom::RhomObject.method_name_reserved?(attr)
            if @@attribs_map[srcid][attr]
                @@attribs_map[srcid][attr] += 1
            else
                @@attribs_map[srcid][attr] = 1
            end
        end    
    end

    def RhomAttribManager.add_attrib(srcid_a,attr)
        @@mxMap.lock
        begin
            #puts 'RhomAttribManager.add_attrib: ' + srcid_a.to_s + "; " + attr.to_s
        
            srcid = srcid_a.to_i
            @@attribs_map[srcid] = {} unless @@attribs_map[srcid]
            _add_attrib(srcid,attr)
        ensure    
            @@mxMap.unlock
        end     
    end

    def RhomAttribManager.add_attribs(srcid_a,attr_arr)
        @@mxMap.lock
        begin
            srcid = srcid_a.to_i
            @@attribs_map[srcid] = {} unless @@attribs_map[srcid]

            attr_arr.each do |attr|
                _add_attrib(srcid,attr)
            end
            
            _save(srcid)    
        ensure    
            @@mxMap.unlock
        end     
    end

    def RhomAttribManager.delete_attribs(srcid_a,cond)
        @@mxMap.lock
        begin
            puts 'RhomAttribManager.delete_attribs: ' + srcid_a.to_s + '; ' + cond.to_s
            
            srcid = srcid_a.to_i
            return unless @@attribs_map[srcid]
            
            isSave = true
            if cond && !cond.is_a?(Hash)
                cond = {'id' => cond}
                isSave = false
            end

            puts 'RhomAttribManager.delete_attribs: ' + @@attribs_map[srcid].to_s            
            
            if !cond
                @@attribs_map[srcid] = {}
            else
                attribs = ::Rhom::RhomDbAdapter.select_from_table('object_values','attrib',cond)
                attribs.each do |attrib|
                    attr = attrib['attrib']
                    
                    if @@attribs_map[srcid][attr]
                        @@attribs_map[srcid][attr] -= 1
                        if @@attribs_map[srcid][attr] == 0
                            @@attribs_map[srcid].delete attr    
                        end
                    end
                end
            end

            puts 'RhomAttribManager.delete_attribs: ' + @@attribs_map[srcid].to_s
            _save(srcid) if isSave
        ensure    
            @@mxMap.unlock
        end     
    end

    def RhomAttribManager._save(srcid)
        #puts 'RhomAttribManager._save: ' + srcid.to_s
        attribs = @@attribs_map[srcid]
        return unless attribs
        
        strAttribs = ""
        attribs.each do |attr,counter|
            strAttribs += ',' if strAttribs.length > 0
            strAttribs += attr + ',' + counter.to_s
        end
        
        #puts 'RhomAttribManager._save : ' + srcid.to_s + '; ' + strAttribs
        ::Rhom::RhomDbAdapter::update_into_table('sources', {"source_attribs"=>strAttribs},
                                           {"source_id"=>srcid})
    end

    def RhomAttribManager.save(srcid_a)
        @@mxMap.lock
        begin
            #puts 'RhomAttribManager.save: ' + srcid_a.to_s
            srcid = srcid_a.to_i
            _save(srcid)
        ensure    
            @@mxMap.unlock
        end     
    end
    
    def RhomAttribManager.load(srcid_a,strAttribs)
        @@mxMap.lock
        begin
            return unless strAttribs
            
            puts 'RhomAttribManager.load: ' + srcid_a.to_s + "; " + strAttribs
            
            srcid = srcid_a.to_i
            arr = strAttribs.split(',')
            attribs = {}
            i = 0
            while i < arr.size
                attribs[arr[i]] = arr[i+1].to_i
                i += 2
            end

            puts 'RhomAttribManager.load: ' + attribs.to_s
            @@attribs_map[srcid] = attribs
        ensure    
            @@mxMap.unlock
        end     
        
    end
    
  end
  
  class RhomObjectFactory
    
    def initialize
	    unless not defined? Rho::RhoConfig.sources
  	    init_objects
  	  end
    end
  
    # Initialize new object with dynamic attributes
    def init_objects
      Rho::RhoConfig.sources.each do |classname,source|
        unless Object.const_defined?(classname.intern)
          Object.const_set(classname.intern, 
            Class.new do
              include ::Rhom::RhomObject
              extend ::Rhom::RhomObject
              
              # This holds the attributes for an instance of
              # the rhom object
              attr_accessor :vars
          
              def initialize(obj=nil)
                @vars = {}
                self.vars['object'] = "#{((Time.now.to_f - Time.mktime(2009,"jan",1,0,0,0,0).to_f) * 10**6).to_i}"
                if obj
                  obj.each do |key,value|
                    self.vars[key] = value
                  end
                end
              end
              
              def method_missing(name, *args)
                unless name == Fixnum
                  varname = name.to_s.gsub(/\=/,"")
                  setting = (name.to_s =~ /=/)
                  if setting
                    @vars[varname] = args[0]  
                  else
                    @vars[varname]
                  end
                end
              end
		  
              class << self
              
                def count
                  #SyncEngine.lock_sync_mutex
                  res = ::Rhom::RhomDbAdapter.select_from_table('object_values','object', {"source_id"=>get_source_id}, {"distinct"=>true}).length
                  #SyncEngine.unlock_sync_mutex
                  res
                end
              
                def get_source_id
                  Rho::RhoConfig.sources[self.name.to_s]['source_id'].to_s
                end
                
                # retrieve a single record if object id provided, otherwise return
                # full list corresponding to factory's source id
                def find(*args)
                  raise ::Rhom::RecordNotFound if args[0].nil? or args.length == 0
                  puts "Inside find: args - #{args.inspect}"
                  ret_list = []
                  conditions = {}
                  where_cond = nil
                  # first find all query objects
                  if args.first == :all || args.first == :first
                    where_cond = {"source_id"=>get_source_id}
                  elsif args.first.is_a?(String)
                    where_cond = {"object"=>strip_braces(args.first.to_s),"source_id"=>get_source_id}
                  end

                  # do we have conditions?
                  # if so, add them to the query
                  condition_hash = nil
                  select_arr = nil
                  condition_str = nil
                  limit = nil
                  offset = nil
                  order_dir=""
                  if args[1]
                    if args[1][:conditions]
                      condition_hash = args[1][:conditions] if args[1][:conditions].is_a?(Hash)
                      # conditions are a string
                      condition_str = args[1][:conditions] if args[1][:conditions].is_a?(String)
                      # conditions are an array
                      if args[1][:conditions].is_a? (Array)
                        condition_str = args[1][:conditions][0].split(/\?/).each_with_index { |param,i| 
                          param << args[1][:conditions][i+1].to_s
                        }.join(' ').to_s 
                      end
                    end
                    if args[1][:per_page] and args[1][:offset]
                      limit = args[1][:per_page].to_s
                      offset = args[1][:offset].to_s
                    end
                    select_arr = args[1][:select] if args[1][:select]
                    order_dir = args[1][:orderdir] if args[1][:orderdir]
                  end
                  
		          start = Time.new
                  # return horizontal resultset from database
                  # for example, an object that has attributes name,industry:
                  # |               object                 |       name         |  industry   |
                  # ---------------------------------------------------------------------------
                  # | 3560c0a0-ef58-2f40-68a5-48f39f63741b |A.G. Parr PLC 37862 |Entertainment|
                  if select_arr
                    attribs = select_arr
                    attribs = attribs | condition_hash.keys if condition_hash
                  else
                    attribs = ::Rhom::RhomAttribManager.get_attribs(get_source_id)
                  end
                    
                  puts "get_attribs took #{Time.new - start} sec"
                  start = Time.new
                  if attribs and attribs.length > 0
                    sql = ""
                    sql << "SELECT * FROM (\n" if condition_hash or condition_str
                    sql << "SELECT object,\n"
                    #attribs.reject! {|attrib| select_arr.index(attrib).nil?} if select_arr
                    attribs.each do |attrib|
                      unless attrib.nil? or attrib.length == 0 or ::Rhom::RhomObject.method_name_reserved?(attrib)
                        sql << "MAX(CASE WHEN attrib = '#{attrib}' THEN value ELSE NULL END) AS \"#{attrib}\",\n"
                      end
                    end 
                    sql.chomp!
                    sql.chop!
                    sql << " FROM object_values ov \n"
                    sql << "where " + ::Rhom::RhomDbAdapter.where_str(where_cond) + "\n" if where_cond and where_cond.length > 0
                    sql << "group by object\n"
                    sql << "order by \"#{args[1][:order]}\" " + order_dir if args[1] and args[1][:order]
                    sql << ") WHERE " + ::Rhom::RhomDbAdapter.where_str(condition_hash) if condition_hash
                    sql << ") WHERE " + condition_str if condition_str
                    sql << " LIMIT " + limit + " OFFSET " + offset if limit and offset

                    puts "sql prepare took #{Time.new - start} sec"
                    start = Time.new
                    list = ::Rhom::RhomDbAdapter.execute_sql(sql)
                    puts "Database query took #{Time.new - start} sec, #{list.length} rows"
                    start = Time.new
                    list.each do |rowhash|
                      # always return object field with surrounding '{}'
                      rowhash['object'] = "{#{rowhash['object']}}"
                      new_obj = self.new
                      new_obj.vars.merge!(rowhash)
                      ret_list << new_obj
                    end
                    
                    puts "Processing rhom objects took #{Time.new - start} sec, #{ret_list.length} objects"
                    
                  end
                  
                  args.first == :first || args.first.is_a?(String) ? ret_list[0] : ret_list
                end
              
                def search(args)
                  searchParams = ""
                  if args[:search_params]
                    args[:search_params].each do |key,value|
                      searchParams += '&' + "conditions[#{Rho::RhoSupport.url_encode(key)}]" + '=' + Rho::RhoSupport.url_encode(value)
                    end  
                  end
                    
                  searchParams += '&offset=' + Rho::RhoSupport.url_encode(args[:offset]) if args[:offset]
                  searchParams += '&max_results=' + Rho::RhoSupport.url_encode(args[:max_results]) if args[:max_results]
                    
                  set_notification(args[:callback], args[:callback_param]) if args[:callback]
                  SyncEngine.dosearch_source(get_source_id.to_i(), args[:from] ? args[:from] : 'search',
                    searchParams)
                end
                
                # Alias for find
                def find_all(args=nil)
                  find(:all, args)
                end
                
                # Returns a set of rhom objects, limiting the set to length :per_page
                # If no :per_page is specified, the default size is 10
                def paginate(args=nil)
                  # Default to 10 items per page
                  args[:page] ||= 0
                  args[:per_page] ||= 10
                  args[:offset] = args[:page] * args[:per_page]
                  find(:all, args)
                end
  
                def set_notification(url,params)
                  SyncEngine.set_notification(get_source_id.to_i,url,params)
                end
                
                def clear_notification
                  SyncEngine.clear_notification(get_source_id.to_i)
                end
                
                def ask(question)
                  tmp_obj = self.new(:object =>djb_hash("#{question}#{rand.to_s}", 10).to_s)
                  if question
                      begin
                          ::Rhom::RhomDbAdapter.start_transaction
                          
                          # We only support one ask at a time!
                          result = ::Rhom::RhomDbAdapter.select_from_table('changed_values', 'object', {"source_id"=>get_source_id, "update_type"=>'ask'}) 
                          if result && result.length > 0 
                            ::Rhom::RhomDbAdapter.delete_from_table('changed_values', {"source_id"=>get_source_id, "update_type"=>'ask'})
                            ::Rhom::RhomDbAdapter.delete_from_table('object_values', {"object"=>result[0]['object'], "attrib"=>'question', "source_id"=>get_source_id} )
                          else
                            ::Rhom::RhomAttribManager.add_attribs(get_source_id,['question'])  
                          end
                          
                          ::Rhom::RhomDbAdapter.insert_into_table('changed_values', {"source_id"=>get_source_id, "object"=>tmp_obj.object, "attrib"=>'question', "value"=>Rho::RhoSupport.url_encode(question), "update_type"=>'ask'} )
                          ::Rhom::RhomDbAdapter.insert_into_table('object_values', {"source_id"=>get_source_id, "object"=>tmp_obj.object, "attrib"=>'question', "value"=>Rho::RhoSupport.url_encode(question)} )
                          
                          #::Rhom::RhomDbAdapter.delete_from_table('object_values', {"source_id"=>get_source_id, "update_type"=>'ask'})
                          #::Rhom::RhomDbAdapter.insert_into_table('object_values', {"source_id"=>get_source_id, "object"=>tmp_obj.object, "attrib"=>'question', "value"=>Rho::RhoSupport.url_encode(question), "update_type"=>'ask'})

                          ::Rhom::RhomDbAdapter.commit
                          
                      rescue Exception => e
                          puts 'Ask Exception: ' + e.inspect
                          ::Rhom::RhomDbAdapter.rollback    
                      end    
                          
                    SyncEngine.dosync
                  end
                end
                
                # deletes all records matching conditions (optionally nil)
                def delete_all(conditions=nil)
                  #update_type= get_update_type_by_source('delete')
                
                  begin
                      ::Rhom::RhomDbAdapter.start_transaction
                      if conditions
                        del_conditions = get_conditions_hash(conditions[:conditions])
                      
                        # find all relevant objects, then delete them
                        del_objects = ::Rhom::RhomDbAdapter.select_from_table('object_values', 'object', del_conditions.merge!({"source_id"=>get_source_id}), {"distinct"=>true})
                        del_objects.each do |obj|                                                       
                          ::Rhom::RhomAttribManager.delete_attribs(get_source_id,{'object'=>obj['object']})
                          ::Rhom::RhomDbAdapter.delete_from_table('object_values', {'object'=>obj['object']})
                          ::Rhom::RhomDbAdapter.delete_from_table('changed_values', {'object'=>obj['object']})
                          #TODO: add delete update_type : get_update_type_by_source is instance method, cannot use it
                          #::Rhom::RhomDbAdapter.insert_into_table('changed_values', {"source_id"=>get_source_id, 'object'=>obj['object'], "update_type"=>update_type})
                        end
                      else
                        ::Rhom::RhomAttribManager.delete_attribs(get_source_id,nil)
                        ::Rhom::RhomDbAdapter.delete_from_table('object_values', {"source_id"=>get_source_id})
                        ::Rhom::RhomDbAdapter.delete_from_table('changed_values', {"source_id"=>get_source_id})
                        #TODO: add delete all update_type
                      end
                      ::Rhom::RhomDbAdapter.commit
 
                  rescue Exception => e
                      puts 'delete_all Exception: ' + e.inspect
                      ::Rhom::RhomDbAdapter.rollback    
                  end    
                      
                end
                    
                private
                
                # returns attributes for the source
                #@@attribs_map = Hash.new
                #def get_attribs
                 # srcid = get_source_id
                  #attribs = @@attribs_map[srcid]
                  #if attribs
                  #  return attribs
                  #end
                  
                  #attribs = ::Rhom::RhomDbAdapter.select_from_table('object_values','attrib', {"source_id"=>srcid}, {"distinct"=>true})
                  #attribs.collect! do |attrib|
                  #  attrib['attrib']
                  #end
                  
                  #@@attribs_map[srcid] = attribs
                  #attribs
                #end
                
                # get hash of conditions in sql form
                def get_conditions_hash(conditions=nil)
                  if conditions
                    condition_hash = {}
                    conditions.each do |key,value|
                      condition_hash.merge!('attrib' => key.to_s, 'value' => value.to_s)
                    end
                    condition_hash
                  else
                    nil
                  end
                end
              end #class methods
	
              # deletes the record from the viewable list as well as
              # adding a delete record to the list of sync operations
              def destroy
                result = nil
                obj = self.inst_strip_braces(self.object)
                update_type=self.get_update_type_by_source('delete')
                
                if obj
                    begin
                      ::Rhom::RhomDbAdapter.start_transaction
                
                      # first delete the record from viewable list
                      ::Rhom::RhomAttribManager.delete_attribs(self.get_inst_source_id,{"object"=>obj})
                      
                      result = ::Rhom::RhomDbAdapter.delete_from_table('object_values', {"object"=>obj})
                      ::Rhom::RhomDbAdapter.delete_from_table('changed_values', {"object"=>obj})
                      if update_type
                        # now add delete operation
                        result = ::Rhom::RhomDbAdapter.insert_into_table('changed_values', {"source_id"=>self.get_inst_source_id, "object"=>obj, "update_type"=>update_type})
                      end
                      
                      ::Rhom::RhomDbAdapter.commit
 
                    rescue Exception => e
                      puts 'destroy Exception: ' + e.inspect
                      ::Rhom::RhomDbAdapter.rollback    
                    end    
                      
                end
                result
              end
		
              # saves the current object to the database as a create type
              def save
                result = nil
                # iterate over each instance variable and insert create row to table
				obj = self.inst_strip_braces(self.object)
				update_type=self.get_update_type_by_source('create')
                begin
                    ::Rhom::RhomDbAdapter.start_transaction
                
                    self.vars.each do |key,value|
                      val = self.inst_strip_braces(value)
                      # add rows excluding object, source_id and update_type
                      unless ::Rhom::RhomObject.method_name_reserved?(key)
                        fields = {"source_id"=>self.get_inst_source_id,
                                  "object"=>obj,
                                  "attrib"=>key,
                                  "value"=>val,
                                  "update_type"=>update_type}
                        fields = key == "image_uri" ? fields.merge!({"attrib_type" => "blob.file"}) : fields
                        
                        ::Rhom::RhomDbAdapter.insert_into_table('changed_values', fields)
                        fields.delete("update_type")
                        result = ::Rhom::RhomDbAdapter.insert_into_table('object_values', fields)                                     
                      end
                    end
                    ::Rhom::RhomAttribManager.add_attribs(self.get_inst_source_id,self.vars.keys)
                    ::Rhom::RhomDbAdapter.commit

                rescue Exception => e
                    puts 'save Exception: ' + e.inspect
                    ::Rhom::RhomDbAdapter.rollback    
                end    
                
                result
              end
          
              # updates the current record in the viewable list and adds
              # a sync operation to update
              def update_attributes(attrs)
                result = nil
                obj = self.inst_strip_braces(self.object)
                update_type=self.get_update_type_by_source('update')
                begin
                    ::Rhom::RhomDbAdapter.start_transaction
                
                    attrs.each do |attrib,val|
                      attrib = attrib.to_s.gsub(/@/,"")
                      old_val = self.send attrib.to_sym unless ::Rhom::RhomObject.method_name_reserved?(attrib)
                      
                      # Don't save objects with braces to database
                      new_val = self.inst_strip_braces(val)
                      
                      # if the object's value doesn't match the database record
                      # then we procede with update
                      if old_val != new_val
                        unless ::Rhom::RhomObject.method_name_reserved?(attrib)
                          # only one update at a time
                          ::Rhom::RhomDbAdapter.delete_from_table('changed_values', {"source_id"=>self.get_inst_source_id, "object"=>obj, "attrib"=>attrib, "update_type"=>update_type})
                          # add to syncengine queue
                          ::Rhom::RhomDbAdapter.insert_into_table('changed_values', {"source_id"=>self.get_inst_source_id, "object"=>obj, "attrib"=>attrib, "value"=>new_val, "update_type"=>update_type})
                          
                          # update viewable ('query') list
                          #::Rhom::RhomDbAdapter.delete_from_table('object_values', {"source_id"=>self.get_inst_source_id, "object"=>obj, "attrib"=>attrib, "update_type"=>'query'})
                          #result = ::Rhom::RhomDbAdapter.insert_into_table('object_values', {"source_id"=>self.get_inst_source_id, "object"=>obj, "attrib"=>attrib, "value"=>new_val, "update_type"=>'query'})

                          result = ::Rhom::RhomDbAdapter.select_from_table('object_values', 'object', {"object"=>obj, "attrib"=>attrib, "source_id"=>self.get_inst_source_id}) 
                          if result && result.length > 0 
                            result = ::Rhom::RhomDbAdapter.update_into_table('object_values', {"value"=>new_val}, {"object"=>obj, "attrib"=>attrib, "source_id"=>self.get_inst_source_id})
                          else
                            result = ::Rhom::RhomDbAdapter.insert_into_table('object_values', {"source_id"=>self.get_inst_source_id, "object"=>obj, "attrib"=>attrib, "value"=>new_val})                          
                            ::Rhom::RhomAttribManager.add_attrib(self.get_inst_source_id,attrib)
                          end    
                        end
                      end
                    end
                    
                    ::Rhom::RhomAttribManager.save(self.get_inst_source_id)
                    ::Rhom::RhomDbAdapter.commit

                rescue Exception => e
                    puts 'update_attributes Exception: ' + e.inspect
                    ::Rhom::RhomDbAdapter.rollback    
                end    
                    
                result
              end
	
              def get_inst_source_id
                Rho::RhoConfig.sources[self.class.name.to_s]['source_id'].to_s
              end
              
              def get_update_type_by_source(update_type)
                source_type = Rho::RhoConfig.sources[self.class.name.to_s]['type']
                if source_type and source_type == "ask" and update_type == 'delete'
                  nil
                elsif source_type and source_type == "ask"
                  "query"
                else
                  update_type
                end
              end

              def inst_strip_braces(str=nil)
                str ? str.gsub(/\{/,"").gsub(/\}/,"") : nil
              end
            end)
        end
      end
    end
  end # RhomObjectFactory
end # Rhom