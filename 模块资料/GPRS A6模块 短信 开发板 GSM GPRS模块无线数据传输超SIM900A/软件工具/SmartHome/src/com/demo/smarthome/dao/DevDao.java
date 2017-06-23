package com.demo.smarthome.dao;

import java.util.ArrayList;
import java.util.List;

import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.util.Log;

import com.demo.smarthome.db.DatabaseHelper;
import com.demo.smarthome.device.Dev;
import com.demo.smarthome.service.DevService;

public class DevDao implements DevService {
	private DatabaseHelper dbHelper = null;

	private static final String TAG = "DevDao";
	private static final String ACTIVITY_TAG = "LogDevDao";

	public DevDao(Context context) {
		dbHelper = new DatabaseHelper(context);
	}

	@Override
	public boolean saveDev(Dev dev) {
		// TODO Auto-generated method stub
		boolean flag = false;
		if (dev == null) {
			return flag;
		}
		if (dev.getId().isEmpty()) {
			return flag;
		}
		SQLiteDatabase db = dbHelper.getWritableDatabase();
		try {
			if (findDevById(db, dev.getId())) { // 有记录 update
				flag = updateDev(db, dev);
			} else { // 无记录 insert
				flag = insertDev(db, dev);
			}
		} catch (Exception e) {

		} finally {
			if (db != null) {
				db.close();
			}
		}

		return flag;
	}

	@Override
	public boolean removeDev(String id) {
		// TODO Auto-generated method stub
		boolean flag = false;
		if (id.isEmpty()) {
			return flag;
		}
		SQLiteDatabase db = dbHelper.getWritableDatabase();
		try {
			db.execSQL("delete from dev where id= ?", new Object[] { id });
			flag = true;
		} catch (Exception e) {

		} finally {
			if (db != null) {
				db.close();
			}
		}

		return flag;
	}

	@Override
	public Dev getDevById(String id) {
		// TODO Auto-generated method stub
		// List<Dev> listDev = new ArrayList<Dev>();
		Log.i(ACTIVITY_TAG, "public Dev getDevById(int id)" + id);
		Dev dev = null;
		SQLiteDatabase db = dbHelper.getWritableDatabase();
		try {
			int index = 0;
			String sqlStr = "SELECT id,name  from dev  where id=? ";// 按id升序
																	// 【desc】降序
			Cursor cursor = db.rawQuery(sqlStr, new String[] { id + "" });
			// if(cursor.moveToNext()){
			if (cursor.moveToFirst()) {
				index = 0;
				dev = new Dev();
				// Log.i(ACTIVITY_TAG,"index:"+index);
				dev.setId(cursor.getString(index++));
				dev.setNickName(cursor.getString(index++));

				Log.i(ACTIVITY_TAG, "end index:" + index);
				// listDev.add(dev);
			}
		} catch (Exception e) {
			dev = null;
			Log.i(ACTIVITY_TAG,
					"public Dev getDevById(int id) catch  Exception:"
							+ e.toString());

		} finally {
			if (db != null) {
				db.close();
			}
		}

		//

		if (dev != null) {
			Log.i(ACTIVITY_TAG,
					"   public Dev getDevById(int id)" + dev.toString());
		} else {
			Log.i(ACTIVITY_TAG, "   public Dev getDevById(int id) dev== null");
		}
		return dev;
	}

	@Override
	public List<Dev> getDevList() {
		// TODO Auto-generated method stub
		List<Dev> listDev = new ArrayList<Dev>();
		SQLiteDatabase db = dbHelper.getWritableDatabase();
		try {
			int index = 0;
			String sqlStr = "SELECT id,name  from dev order by id";// 按id升序
																	// 【desc】降序
			Cursor cursor = db.rawQuery(sqlStr, null);
			while (cursor.moveToNext()) {
				index = 0;
				Dev dev = new Dev();
				dev.setId(cursor.getString(index++));
				dev.setNickName(cursor.getString(index++));
				listDev.add(dev);
			}
		} catch (Exception e) {

		} finally {
			if (db != null) {
				db.close();
			}
		}

		return listDev;
	}

	@Override
	public int findDevMaxId() {
		// TODO Auto-generated method stub

		Log.i(TAG, "=====================findDevMaxId");
		int id = -1;
		SQLiteDatabase db = dbHelper.getWritableDatabase();
		try {
			Cursor cursor = db.rawQuery("SELECT id from dev order by id desc",
					null);
			if (cursor.moveToFirst()) {
				id = cursor.getInt(0);
				Log.i(TAG, "=====================findDevMaxId id:" + id);
			}
		} catch (Exception e) {

			Log.i(TAG, "=====================findDevMaxId Exception:" + e);

		} finally {
			if (db != null) {
				db.close();
			}
		}
		Log.i(TAG, "=====================findDevMaxId return id:" + id);
		return id;
	}

	@Override
	public boolean findDevById(int id) {
		// TODO Auto-generated method stub
		boolean findOk = true;
		SQLiteDatabase db = dbHelper.getWritableDatabase();
		try {
			findOk = findDevById(db, id);
		} catch (Exception e) {
			findOk = true;
		} finally {
			if (db != null) {
				db.close();
			}
		}
		return findOk;
	}

	/**
	 * 根据ID,查找一条记录的id,没有的话，返回false
	 * 
	 * @param true
	 * @return
	 * */
	private boolean findDevById(SQLiteDatabase db, int id) {
		// SQLiteDatabase db = helper.getReadableDatabase();
		// Cursor cursor = db.rawQuery("select * from person where id = ?", new
		// String[]{name});
		// int id =-1;
		boolean flag = false;
		if ((db == null) | (id <= 0)) {
			return flag;
		}
		Cursor cursor = db.rawQuery("select id from dev where id = ?",
				new String[] { id + "" });
		if (cursor.moveToFirst()) {
			id = cursor.getInt(0);
			if (id > 0) {
				flag = true;
			}
		}
		return flag;
	}

	/**
	 * 根据ID,查找一条记录的id,没有的话，返回false
	 * 
	 * @param true
	 * @return
	 * */
	private boolean findDevById(SQLiteDatabase db, String id) {
		// SQLiteDatabase db = helper.getReadableDatabase();
		// Cursor cursor = db.rawQuery("select * from person where id = ?", new
		// String[]{name});
		// int id =-1;
		boolean flag = false;
		if ((db == null) | (id.isEmpty())) {
			return flag;
		}
		Cursor cursor = db.rawQuery("select id from dev where id = ?",
				new String[] { id + "" });
		if (cursor.moveToFirst()) {
			id = cursor.getString(0);
			if (!id.isEmpty()) {
				flag = true;
			}
		}
		return flag;
	}

	private boolean updateDev(SQLiteDatabase db, Dev dev) {
		boolean flag = false;
		String id = dev.getId();

		Log.i(ACTIVITY_TAG,
				"   private boolean updateDev(SQLiteDatabase db, Dev dev)"
						+ dev.toString());
		if ((db == null) | (id.isEmpty())) {
			return flag;
		}
		try {
			// db.execSQL("update person set name=? ,phone=? where id=?",new
			// Object[]{name,phone,id});
			db.execSQL("update  dev set " + "name=?  where id=? ",
					new Object[] { dev.getNickName(), dev.getId() });
			flag = true;
		} catch (Exception e) {
			flag = false;
		}
		return flag;
	}

	private boolean insertDev(SQLiteDatabase db, Dev dev) {
		Log.i(ACTIVITY_TAG,
				"   private boolean insertDev(SQLiteDatabase db, Dev dev)"
						+ dev.toString());
		boolean flag = false;
		String id = dev.getId();
		if ((db == null) | (id.isEmpty())) {
			return flag;
		}
		try {
			db.execSQL("insert into dev (" + "id,name) values (?,?)",
					new Object[] { dev.getId(), dev.getNickName() });
			flag = true;
		} catch (Exception e) {
			flag = false;
		}
		return flag;

	}
}
