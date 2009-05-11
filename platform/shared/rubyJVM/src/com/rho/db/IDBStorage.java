package com.rho.db;

public interface IDBStorage {

	public abstract void open(String strPath, String strSqlScript)throws DBException;
	public abstract void close()throws DBException;
	
	public abstract IDBResult executeSQL(String strStatement, Object[] values)throws DBException;
	public abstract IDBResult createResult();
	
	public abstract void deleteAllFiles(String strPath)throws Exception;
	
	public abstract void startTransaction()throws DBException;
	public abstract void commit()throws DBException;
	
	public abstract void setDbCallback(IDBCallback callback);
}
