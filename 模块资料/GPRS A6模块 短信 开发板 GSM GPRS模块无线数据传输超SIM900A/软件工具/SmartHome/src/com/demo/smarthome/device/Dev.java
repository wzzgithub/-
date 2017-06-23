package com.demo.smarthome.device;

/**
 * 设备类
 * 
 * @author Administrator
 * 
 */
public class Dev
{
	private String id;
	private String nickName = "";
	private String lastUpdate = "";
	private String torken = "";
	private String ipPort = "";
	private String pass = "";
	private int errorCount = 0;
	private final int ERROR_COUNT_MAX = 180;
	private boolean dataChange;

	public void runStep()
	{
		if (errorCount >= ERROR_COUNT_MAX)
		{
			this.onLine = false;
		}
		else
		{
			errorCount++;
		}
	}

	public boolean isDataChange()
	{
		return this.dataChange;
	}

	public void isDataChange(boolean ok)
	{
		this.dataChange = ok;
		if (ok)
		{
			onLine = ok;
			errorCount = 0;
		}
	}

	private int lampRVal;

	public int getLampRVal()
	{
		return lampRVal;
	}

	public void setLampRVal(int lampRVal)
	{
		if (lampRVal < 0)
		{
			lampRVal = 0;
		}
		if (lampRVal > 99)
		{
			lampRVal = 99;
		}
		this.lampRVal = lampRVal;
		isDataChange(true);
	}

	public int getLampGVal()
	{
		return lampGVal;
	}

	public void setLampGVal(int lampGVal)
	{
		if (lampGVal < 0)
		{
			lampGVal = 0;
		}
		if (lampGVal > 99)
		{
			lampGVal = 99;
		}
		this.lampGVal = lampGVal;
		isDataChange(true);
	}

	public int getLampBVal()
	{
		return lampBVal;
	}

	public void setLampBVal(int lampBVal)
	{
		if (lampBVal < 0)
		{
			lampBVal = 0;
		}
		if (lampBVal > 99)
		{
			lampBVal = 99;
		}
		this.lampBVal = lampBVal;
		isDataChange(true);
	}

	private int lampGVal;
	private int lampBVal;

	public String getPass()
	{
		return pass;
	}

	public void setPass(String pass)
	{
		this.pass = pass;
	}

	private double temp = 0;
	private boolean lightState = false;

	public double getTemp()
	{
		return temp;
	}

	public void setTemp(double temp)
	{
		this.temp = temp;
		isDataChange(true);
	}

	public boolean isLightState()
	{
		return lightState;
	}

	public void setLightState(boolean lightState)
	{
		this.lightState = lightState;
		isDataChange(true);
	}

	public String getTorken()
	{
		return torken;
	}

	public void setTorken(String torken)
	{
		this.torken = torken;
	}

	public String getIpPort()
	{
		return ipPort;
	}

	public void setIpPort(String ipPort)
	{
		this.ipPort = ipPort;
	}

	public String getLastUpdate()
	{
		return lastUpdate;
	}

	public void setLastUpdate(String lastUpdate)
	{
		this.lastUpdate = lastUpdate;
	}

	private boolean onLine = false;

	public String getId()
	{
		return id;
	}

	public void setId(String id)
	{
		this.id = id;
	}

	public String getNickName()
	{
		return nickName;
	}

	public void setNickName(String nickName)
	{
		this.nickName = nickName;
	}

	@Override
	public String toString()
	{
		return "id:" + id + " " + (onLine ? "在线" : "不在线") + "  nickName:"
				+ nickName + "  lastUpdate:" + lastUpdate + "  torken:"
				+ torken + "  ipPort:" + ipPort;
	}

	public boolean isOnLine()
	{
		return onLine;
	}

	public void setOnLine(boolean onLine)
	{
		this.onLine = onLine;
	}

}
