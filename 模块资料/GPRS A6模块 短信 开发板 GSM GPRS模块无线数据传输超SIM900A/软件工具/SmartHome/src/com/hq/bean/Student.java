package com.hq.bean;

public class Student {
	private String name;
	private int age;
	private double height;
	
	public String getName() {
		return name;
	}
	public void setName(String name) {
		this.name = name;
	}
	public int getAge() {
		return age;
	}
	public void setAge(int age) {
		this.age = age;
	}
	public double getHeight() {
		return height;
	}
	public void setHeight(double height) {
		this.height = height;
	}
	
	//用于测试，用于在jni中回调
	public void setStudentInfo(String name, double height){
		this.name = name;
		this.height = height;
	}
	
	
}
