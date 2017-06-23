package com.demo.smarthome.db;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteDatabase.CursorFactory;
import android.database.sqlite.SQLiteOpenHelper;

/**
 * DatabaseHelper作为一个访问SQLite的助手类，提供两个方面的功能，
 * 第一，getReadableDatabase(),getWritableDatabase
 * ()可以获得SQLiteDatabse对象，通过该对象可以对数据库进行操作
 * 第二，提供了onCreate()和onUpgrade()两个回调函数，允许我们在创建和升级数据库时，进行自己的操作
 * 
 * @author Administrator
 * 
 */
public class DatabaseHelper extends SQLiteOpenHelper {

	private static final int VERSION = 1;

	private final static String dbFileName = "smarthome.db";

	public DatabaseHelper(Context context) {
		this(context, dbFileName, null, VERSION);
	}

	public DatabaseHelper(Context context, String name) {
		this(context, name, null, VERSION);
	}

	// 在SQLiteOepnHelper的子类当中，必须有该构造函数
	public DatabaseHelper(Context context, String name, CursorFactory factory,
			int version) {
		// 必须通过super调用父类当中的构造函数
		super(context, name, factory, version);
		// TODO Auto-generated constructor stub
	}

	@Override
	public void onCreate(SQLiteDatabase db) {
		db.execSQL(" CREATE TABLE syscfg(  key varchar(200)  ,   value varchar(200)  ,  def_value varchar(200)	);    ");
		db.execSQL(" CREATE TABLE [dev] ( [id]  varchar(20), [name] varchar(20)  );    ");

	}

	// 数据库文件的版本号在更新的时候调用,所以为了添加新的字段，要修改版本号
	@Override
	public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
		// TODO Auto-generated method stub

	}

}
