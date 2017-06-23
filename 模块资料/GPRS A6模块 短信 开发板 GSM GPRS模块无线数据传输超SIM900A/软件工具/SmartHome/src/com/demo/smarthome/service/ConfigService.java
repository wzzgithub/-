package com.demo.smarthome.service;

/**
 * 数据库保存的配置信息 服务接口
 * 
 * @author Administrator
 * 
 */
public interface ConfigService {
	public String getCfgByKey(String key);

	public boolean SaveSysCfgByKey(String key, String value);
}
