package com.hq.bean;

public class LedNative {
	//1�����ض�̬��
	static{
		System.loadLibrary("myndk_jni"); //  /system/lib/libled_jni.so
	}
	
	//2������native����
	public native int openDev();
	public native int devOn();
	public native int devOff();
	public native int closeDev();
	
	//�������ط���--��������
	public native int GetPWD(long a);
	
	//�����ַ���
	public native String getStringFromJni();
	public native void setStringToJni(String str);
	
	//���ݶ���
	public native Student getStudent();
	public native void setStudent(Student stu);
	
}
