require 'mspec'
require 'mspec/utils/script'
require 'mspec/version'

class SpecRunner < MSpecScript
  def initialize
    super
    config[:files] = []
    
    # turn on exception backtrace
    MSpec.backtrace = true
    
    config[:files] << 'spec/rhom_spec'
    config[:files] << 'spec/rhoruby_spec'
    config[:files] << 'spec/rho_controller_spec'
    config[:files] << 'spec/rho_spec'
    config[:files] << 'spec/spec_helper'
    config[:files] << 'spec/webview_spec'
    
  end

  def run
    MSpec.register_files config[:files]

    MSpec.process
    MSpec.exit_code
  end
end