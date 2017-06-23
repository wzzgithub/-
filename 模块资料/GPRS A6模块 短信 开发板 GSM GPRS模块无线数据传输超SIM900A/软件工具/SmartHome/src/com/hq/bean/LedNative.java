package com.hq.bean;

public class LedNative {
	//1，加载动态库
	static{
		System.loadLibrary("myndk_jni"); //  /system/lib/libled_jni.so
	}
	
	//2，声明native方法
	public native int openDev();
	public native int devOn();
	public native int devOff();
	public native int closeDev();
	
	//新增本地方法--传递数据
	public native int GetPWD(long a);
	
	//传递字符串
	public native String getStringFromJni();
	public native void setStringToJni(String str);
	
	//传递对象
	public native Student getStudent();
	public native void setStudent(Student stu);
	
}
