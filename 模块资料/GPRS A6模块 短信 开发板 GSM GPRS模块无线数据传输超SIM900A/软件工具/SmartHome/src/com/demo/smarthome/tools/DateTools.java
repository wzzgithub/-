package com.demo.smarthome.tools;

import java.text.DateFormat;
import java.text.SimpleDateFormat;

import java.util.Date;

/**
 * 日期工具类
 * 
 * @author Administrator
 * 
 */
public class DateTools {
	/**
	 * 得到当前时间 "yyyy-MM-dd HH:mm:ss"
	 * 
	 * @return
	 */
	public static String getNowTimeString() {
		// String result = "";
		// Date now = new Date();
		// Calendar cal = Calendar.getInstance();
		//
		// DateFormat d1 = DateFormat.getDateInstance();
		// //默认语言（汉语）下的默认风格（MEDIUM风格，比如：2008-6-16 20:54:53）
		// result = d1.format(now);
		// SimpleDateFormat format=new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		return new SimpleDateFormat("yyyy-MM-dd HH:mm:ss").format(new Date());
	}

	/**
	 * 当前时间到strTime的时间差 秒数
	 * 
	 * @param strTime
	 *            2004-03-26 13:31:40
	 * @return
	 */
	public static long getNowTimeByLastTimeDifference(String strTime) {
		long timeVal = -1;
		DateFormat df = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");

		try {
			Date d1 = new Date(System.currentTimeMillis());// 获取当前时间
			Date d2 = df.parse(strTime);
			timeVal = d1.getTime() - d2.getTime();
			timeVal /= 1000;
			System.out.println("newTime:" + d1.toString() + "  d2:"
					+ d2.toString() + "   strTime:" + strTime + "    val:"
					+ timeVal);
		} catch (Exception e) {
		}

		return timeVal;
	}

}
