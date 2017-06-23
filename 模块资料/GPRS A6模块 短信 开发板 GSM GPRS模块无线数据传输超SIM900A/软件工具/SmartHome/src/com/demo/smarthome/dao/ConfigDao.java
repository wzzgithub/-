package com.demo.smarthome.dao;

import com.demo.smarthome.db.DatabaseHelper;
import com.demo.smarthome.service.ConfigService;

import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;

/**
 * 数据库配置信息实现类
 * 
 * @author Administrator
 * 
 */
public class ConfigDao implements ConfigService {

	private String ACTIVITY_TAG = "ConfigDao";
	private DatabaseHelper dbHelper = null;

	public ConfigDao(Context context) {
		dbHelper = new DatabaseHelper(context);
	}

	/**
	 * 通过键得到对应的值
	 */
	@Override
	public String getCfgByKey(String key) {
		// Log.i(ACTIVITY_TAG, "public String getCfgByKey(String key) key:" +
		// key);
		String value = "";
		SQLiteDatabase db = dbHelper.getReadableDatabase();
		try {
			int index = 0;
			String sqlStr = "SELECT value from syscfg where key =? ";
			Cursor cursor = db.rawQuery(sqlStr, new String[] { key });
			if (cursor.moveToFirst()) {
				index = 0;
				value = cursor.getString(index++);
			}
		} catch (Exception e) {
			value = "";
			// Log.i(ACTIVITY_TAG,"public String getCfgByKey(String key)catch  Exception:"+
			// e.toString());
		} finally {
			if (db != null) {
				db.close();
			}
		}

		if (value == null) {
			value = "";
		}

		// Log.i(ACTIVITY_TAG, "public String getCfgByKey(String key) key" +
		// key+ "  value:" + value);
		return value;
	}

	/**
	 * 保存键值对
	 */
	@Override
	public boolean SaveSysCfgByKey(String key, String value) {
		// Log.i(ACTIVITY_TAG,
		// "public boolean SaveSysCfgByKey(String key, String value key"+ key +
		// "  value:" + value);
		boolean flag = false;
		if (key == null) {
			return flag;
		}

		if (key == "") {
			return flag;
		}
		if (value == null) {
			value = "";
		}

		SQLiteDatabase db = dbHelper.getWritableDatabase();
		try {
			if (findConfigByKey(db, key)) { // 有记录
				// update
				flag = updateConfig(db, key, value);
			} else { // 无记录 insert
				flag = insertConfig(db, key, value);
			}
			flag = true;
		} catch (Exception e) {

		} finally {
			if (db != null) {
				db.close();
			}
		}

		return flag;
	}

	// =====
	/**
	 * 根据ID,查找一条记录的id,没有的话，返回false
	 * 
	 * @param true
	 * @return
	 * */
	private boolean findConfigByKey(SQLiteDatabase db, String key) {

		boolean flag = false;
		if (db == null) {
			return flag;
		}
		Cursor cursor = db.rawQuery("select  value from syscfg where key= ? ",
				new String[] { key });
		if (cursor.moveToFirst()) {
			String value = cursor.getString(0);
			if (value != null) {
				if (value.trim() != "") {
					flag = true;
				}
			}
		}
		return flag;
	}

	/**
	 * 更新数据库中的键值对
	 * 
	 * @param db
	 *            数据库
	 * @param key
	 *            键
	 * @param value
	 *            值
	 * @return
	 */
	private boolean updateConfig(SQLiteDatabase db, String key, String value) {
		boolean flag = false;

		// Log.i(ACTIVITY_TAG,
		// " private boolean updateConfig(SQLiteDatabase db, String key, String value)  key:"+
		// key + "   value:" + value);
		if (db == null) {
			return flag;
		}
		try {
			db.execSQL("update  syscfg set " + "value=?  where key=?",
					new Object[] { value, key });
			flag = true;
		} catch (Exception e) {
			flag = false;
		}
		return flag;
	}

	/**
	 * 插入数据库中的键值对
	 * 
	 * @param db
	 * @param key
	 * @param value
	 * @return
	 */
	private boolean insertConfig(SQLiteDatabase db, String key, String value) {
		// Log.i(ACTIVITY_TAG,
		// "   private boolean insertConfig(SQLiteDatabase db, String key, String value)   key:"+
		// key + "   value:" + value);
		boolean flag = false;
		if (db == null) {
			return flag;
		}
		try {
			db.execSQL("insert into syscfg (" + "key,value ) values (?,?)",
					new Object[] { key, value });
			flag = true;
		} catch (Exception e) {
			flag = false;
		}
		return flag;
	}
}
