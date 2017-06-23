package com.demo.smarthome.protocol;

import java.util.ArrayList;
import java.util.List;

import android.content.Intent;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;

import com.demo.smarthome.device.Dev;
import com.demo.smarthome.iprotocol.IProtocol;
import com.demo.smarthome.service.Cfg;
import com.demo.smarthome.service.SocketService;
import com.demo.smarthome.tools.DateTools;
import com.demo.smarthome.tools.IpTools;
import com.demo.smarthome.tools.StrTools;

/**
 * 协议类
 *
 * @author Administrator
 *
 */
public class PlProtocol implements IProtocol
{
	private int packLeng = 21;
	private static final String TAG = "PlProtocol";

	/**
	 * MessageEnCode
	 */
	public boolean MessageEnCode(Msg msg)
	{
		if (msg == null)
		{
			return false;
		}
		int len = msg.getDataLen();// + packLeng+msg.getTorken().length+1;
		len += packLeng + 1;
		if (msg.getTorken() != null)
		{
			len += msg.getTorken().length;
		}
		long value = 0;
		long val = 0;
		int index = 0;
		int i = 0;
		byte[] buff;
		byte[] torken;
		int buffLength = 0;
		buff = new byte[len];
		byte[] da;
		int daLength = 0;
		da = new byte[len * 2];

		// buff[index++] = 0x55;
		// 长度
		buff[index++] = (byte) (len % 256);
		buff[index++] = (byte) (len / 256);
		// 命令类别
		buff[index++] = (byte) ((msg.getCmdType().val()) % 256);
		// 命令字
		buff[index++] = (byte) (msg.getCmd().val() % 256);
		// 扩展信息 包括序号，加密等其它扩展
		buff[index++] = 0;
		buff[index++] = 0;
		buff[index++] = 0;
		buff[index++] = 0;
		// 状态
		buff[index++] = (byte) (msg.getState().val());

		// 设备ID
		torken = msg.getId();
		val = torken.length;

		for (int j = 0; j < 8; j++)
		{
			if (j < val)
			{
				buff[index++] = (byte) (torken[j] % 256);
			}
			else
			{
				buff[index++] = 0;
			}
		}

		// Torken 通信令牌，登录成功后才有此字段,第一字节表示长度
		torken = msg.getTorken();
		if (torken == null)
		{
			val = 0;
		}
		else
		{
			val = torken.length;
		}

		buff[index++] = (byte) (val % 256);
		for (int j = 0; j < val; j++)
		{
			buff[index++] = (byte) (torken[j] % 256);
		}

		if (msg.getDataLen() > 0)
		{
			System.arraycopy(msg.getData(), 0, buff, index, msg.getDataLen());
			// System.arraycopy(msg.getData(), 0, buff, index,
			// msg.getDataLen());
			index += msg.getDataLen();
		}

		buffLength = index;

		//
		byte[] b = new byte[index + 1];
		b[0] = 0x55;
		for (int m = 1; m <= index; m++)
		{
			b[m] = buff[m - 1];
		}
		// cs
		CRC16 crc = new CRC16();
		crc.reset();
		crc.update(b, 0, index + 1);
		long cs = crc.getValue();

		daLength = 0;
		index = 0;
		da[index++] = 0x55;
		for (i = 0; i < buffLength; i++, index++)// 编码 55=> 54 01 54=> 54 02
		{
			da[index] = buff[i];
			if (buff[i] == 0x55)
			{
				da[index] = 0x54;
				index++;
				da[index] = 0x01;
			}
			else if (buff[i] == 0x54)
			{
				da[index] = 0x54;
				index++;
				da[index] = 0x02;
			}

		}
		// da.data[index++] = buff[i];
		daLength = index;

//		da[daLength++] = (byte) (cs / 256);
//		da[daLength++] = (byte) (cs % 256);
		byte csVal=(byte) (cs / 256);
		if(csVal== 0x54)
		{
			da[daLength++] =0x54;
			da[daLength++] =0x02;
		}
		else if(csVal== 0x55)
		{
			da[daLength++] =0x54;
			da[daLength++] =0x01;
		}
		else
		{
			da[daLength++] =csVal;
		}
		csVal=(byte) (cs % 256);
		if(csVal== 0x54)
		{
			da[daLength++] =0x54;
			da[daLength++] =0x02;
		}
		else if(csVal== 0x55)
		{
			da[daLength++] =0x54;
			da[daLength++] =0x01;
		}
		else
		{
			da[daLength++] =csVal;
		}
		// 包尾
		da[daLength++] = 0x55;
		msg.setSendData(new byte[daLength]);
		System.arraycopy(da, 0, msg.getSendData(), 0, daLength);
		return true;
	}

	// / <summary>
	// / 封装发送数据
	// / </summary>
	// / <param name="msg"></param>
	// / <param name="listStr"></param>
	// / <returns></returns>
	public boolean MessagePackData(Msg msg, String[] listStr)
	{
		boolean ok = false;
		MessagePackDataBefore(msg);

		if (MSGCMDTYPE.CMDTYPE_A0 == msg.getCmdType())
		{
			ok = MessagePackDataA0(msg, listStr);
		}
		else if (MSGCMDTYPE.CMDTYPE_AA == msg.getCmdType())
		{
			ok = MessagePackDataAA(msg, listStr);
		}
		else if (MSGCMDTYPE.CMDTYPE_AC == msg.getCmdType())
		{
			ok = MessagePackDataAC(msg, listStr);
		}
		else if (MSGCMDTYPE.CMDTYPE_F0 == msg.getCmdType())
		{
			ok = MessagePackDataF0(msg, listStr);
		}
		else if (MSGCMDTYPE.CMDTYPE_EF == msg.getCmdType())
		{
			ok = MessagePackDataEF(msg, listStr);
		}

		MessagePackDataAfter(msg);
		return ok;
	}

	private void MessagePackDataBefore(Msg msg)
	{

	}

	private void MessagePackDataAfter(Msg msg)
	{
		MessageEnCode(msg);
	}

	private boolean MessagePackDataA0(Msg msg, String[] listStr)
	{
		boolean ok = false;

		if (MSGCMD.CMD00 == msg.getCmd())
		{
			ok = MessagePackDataA000(msg, listStr);
		}
		else if (MSGCMD.CMD01 == msg.getCmd())
		{
			ok = MessagePackDataA001(msg, listStr);
		}
		return ok;
	}

	private boolean MessagePackDataA000(Msg msg, String[] listStr)
	{
		boolean ok = false;
		boolean error = false;
		String str;
		long val = 0;
		int index = 0;
		int strIndex = 0;
		// IPAddress address;
		byte[] b;

		if (listStr == null)
		{
			return ok;
		}
		// listStr.length
		if (listStr.length != 5)
		{
			return ok;
		}
		byte[] data = new byte[15];
		// dhcp
		str = listStr[strIndex++];
		// data[index++] =
		val = StrTools.stringToInt(str);
		if ((val >= 0) && (val <= 1))
		{
			data[index++] = (byte) (val % 256);
		}
		else
		{
			error = true;
		}

		// ip
		str = listStr[strIndex++];
		if (str == null)
		{
			error = true;
		}
		b = IpTools.getIpV4Byte(str);
		System.arraycopy(b, 0, data, index, 4);
		index += 4;

		// 子网掩码
		str = listStr[strIndex++];
		if (str == null)
		{
			error = true;
		}
		b = IpTools.getIpV4Byte(str);
		System.arraycopy(b, 0, data, index, 4);
		index += 4;

		// 默认网关
		str = listStr[strIndex++];
		if (str == null)
		{
			error = true;
		}
		b = IpTools.getIpV4Byte(str);
		System.arraycopy(b, 0, data, index, 4);
		index += 4;

		// 端口
		str = listStr[strIndex++];
		val = StrTools.stringToInt(str);
		if ((val >= 0) && (val <= 65535))
		{
			data[index++] = (byte) (val / 256);
			data[index++] = (byte) (val % 256);

		}
		else
		{
			error = true;
		}

		if (!error) //
		{
			msg.setDataLen(data.length);
			msg.setData(data);
			msg.setCheckOk(true);
			// MessageEnCode(msg);
			ok = true;
		}
		return ok;
	}

	private boolean MessagePackDataA001(Msg msg, String[] listStr)
	{
		boolean ok = false;
		boolean error = false;
		String str;
		long val = 0;
		int index = 0;
		int strIndex = 0;
		// IPAddress address;
		byte[] b;

		if (listStr == null)
		{
			return ok;
		}
		if (listStr.length != 6)
		{
			return ok;
		}
		byte[] data = new byte[42];
		// 参数个数
		data[index++] = 0x06;
		data[index++] = 0;
		// 主服务器IP地址

		data[index++] = 0x01;
		data[index++] = 0;
		str = listStr[strIndex++];
		if (str == null)
		{
			error = true;
		}
		b = IpTools.getIpV4Byte(str);
		System.arraycopy(b, 0, data, index, 4);
		index += 4;
		// 端口

		data[index++] = 0x02;
		data[index++] = 0;
		str = listStr[strIndex++];
		val = StrTools.stringToInt(str);
		if ((val >= 0) && (val <= 65535))
		{
			data[index++] = (byte) (val / 256);
			data[index++] = (byte) (val % 256);

		}
		else
		{
			error = true;
		}
		// 主服务器密码

		data[index++] = 0x03;
		data[index++] = 0;
		str = listStr[strIndex++];
		index += 8;

		// 备用服务器IP地址

		data[index++] = 0x04;
		data[index++] = 0;
		str = listStr[strIndex++];
		if (str == null)
		{
			error = true;
		}
		b = IpTools.getIpV4Byte(str);
		System.arraycopy(b, 0, data, index, 4);
		index += 4;

		// 端口

		data[index++] = 0x05;
		data[index++] = 0;
		str = listStr[strIndex++];
		val = StrTools.stringToInt(str);
		if ((val >= 0) && (val <= 65535))
		{
			data[index++] = (byte) (val / 256);
			data[index++] = (byte) (val % 256);

		}
		else
		{
			error = true;
		}

		data[index++] = 0x06;
		data[index++] = 0;

		if (!error) //
		{
			msg.setDataLen(data.length);
			msg.setData(data);
			msg.setCheckOk(true);
			// MessageEnCode(msg);
			ok = true;
		}
		return ok;
	}

	private boolean MessagePackDataAA(Msg msg, String[] listStr)
	{
		boolean ok = false;

		if (MSGCMD.CMDEE == msg.getCmd())
		{
			ok = MessagePackDataAAEE(msg, listStr);
		}
		else if (MSGCMD.CMDFF == msg.getCmd())
		{
			ok = MessagePackDataAAFF(msg, listStr);
		}
		else if (MSGCMD.CMD00 == msg.getCmd())
		{
			ok = MessagePackDataAA00(msg, listStr);
		}
		return ok;
	}

	private boolean MessagePackDataAAEE(Msg msg, String[] listStr)
	{
		boolean ok = false;
		// String str;

		Log.i(TAG, "MessagePackDataAAEE");
		long val = 0;
		int index = 0;
		int strIndex = 0;
		if (listStr == null)
		{
			return ok;
		}
		if (listStr.length != 169)
		{
			return ok;
		}

		ok = true;
		index = 0;
		byte[] data = new byte[169];
		for (int i = 0; i < 169; i++)
		{
			val = StrTools.stringToInt(listStr[strIndex++]);
			val = StrTools.intToBcdInt(val);
			data[index++] = (byte) (val % 256);
		}
		msg.setData(data);
		msg.setDataLen(index);
		msg.setCheckOk(true);
		// MessageEnCode(msg);
		return ok;
	}

	private boolean MessagePackDataAAFF(Msg msg, String[] listStr)
	{
		boolean ok = false;
		boolean error = false;
		// String str;
		long val = 0;
		int index = 0;
		int strIndex = 0;
		// IPAddress address;
		// byte[] b;

		if (listStr == null)
		{
			return ok;
		}
		if (listStr.length != 833)
		{
			return ok;
		}
		byte[] data = new byte[833];
		for (int i = 0; i < 833; i++)
		{
			val = StrTools.stringToInt(listStr[strIndex++]);
			val = StrTools.intToBcdInt(val);
			data[index++] = (byte) (val % 256);
		}
		if (!error) //
		{
			msg.setDataLen(data.length);
			msg.setData(data);
			msg.setCheckOk(true);
			// MessageEnCode(msg);
			ok = true;
		}
		return ok;
	}

	private boolean MessagePackDataAA00(Msg msg, String[] listStr)
	{
		boolean ok = false;
		boolean error = false;
		// String str;
		long val = 0;
		int index = 0;
		int strIndex = 0;
		// IPAddress address;
		// byte[] b;

		if (listStr == null)
		{
			return ok;
		}
		if (listStr.length == 2)
		{
			byte[] data = new byte[2];

			val = StrTools.stringToInt(listStr[strIndex++]);
			val = StrTools.intToBcdInt(val);
			data[index++] = (byte) (val % 256);

			val = StrTools.stringToInt(listStr[strIndex++]);
			data[index++] = (byte) (val % 256);

			if (!error) //
			{
				msg.setDataLen(data.length);
				msg.setData(data);
				msg.setCheckOk(true);
				ok = true;
			}
		}
		if (listStr.length == 259)
		{
			byte[] data = new byte[259];

			val = StrTools.stringToInt(listStr[strIndex++]);
			val = StrTools.intToBcdInt(val);
			data[index++] = (byte) (val % 256);

			for (int i = 1; i < 259; i++)
			{
				val = StrTools.stringToInt(listStr[strIndex++]);
				data[index++] = (byte) (val % 256);
			}
			if (!error) //
			{
				msg.setDataLen(data.length);
				msg.setData(data);
				msg.setCheckOk(true);
				ok = true;
			}
		}
		return ok;
	}

	private boolean MessagePackDataAC(Msg msg, String[] listStr)
	{
		boolean ok = false;

		if (MSGCMD.CMD01 == msg.getCmd())
		{
			ok = MessagePackDataAC01(msg, listStr);
		}
		else if (MSGCMD.CMD02 == msg.getCmd())
		{
			ok = MessagePackDataAC02(msg, listStr);
		}
		return ok;
	}

	private boolean MessagePackDataAC01(Msg msg, String[] listStr)
	{
		boolean ok = false;
		// String str;
		long val = 0;
		int index = 0;
		int strIndex = 0;
		if (listStr == null)
		{
			return ok;
		}
		if (listStr.length != 7)
		{
			return ok;
		}

		ok = true;
		index = 0;
		byte[] data = new byte[7];
		for (int i = 0; i < 7; i++)
		{
			val = StrTools.stringToInt(listStr[strIndex++]);
			data[index++] = (byte) (val % 256);
		}

		msg.setData(data);
		msg.setDataLen(index);
		msg.setCheckOk(true);
		// MessageEnCode(msg);
		return ok;
	}

	private boolean MessagePackDataAC02(Msg msg, String[] listStr)
	{
		boolean ok = false;
		// String str;
		long val = 0;
		int index = 0;
		int strIndex = 0;
		if (listStr == null)
		{
			return ok;
		}
		if (listStr.length != 7)
		{
			return ok;
		}

		ok = true;
		index = 0;
		byte[] data = new byte[7];
		for (int i = 0; i < 7; i++)
		{
			val = StrTools.stringToInt(listStr[strIndex++]);
			data[index++] = (byte) (val % 256);
		}

		msg.setData(data);
		msg.setDataLen(index);
		msg.setCheckOk(true);
		// MessageEnCode(msg);
		return ok;
	}

	private boolean MessagePackDataF0(Msg msg, String[] listStr)
	{
		boolean ok = false;

		if (MSGCMD.CMD55 == msg.getCmd())
		{
			ok = MessagePackDataF055(msg, listStr);
		}
		else if (MSGCMD.CMD56 == msg.getCmd())
		{
			ok = MessagePackDataF056(msg, listStr);
		}
		else if (MSGCMD.CMD57 == msg.getCmd())
		{
			ok = MessagePackDataF057(msg, listStr);
		}
		else if (MSGCMD.CMDAA == msg.getCmd())
		{
			ok = MessagePackDataF0AA(msg, listStr);
		}
		else if (MSGCMD.CMDAB == msg.getCmd())
		{
			ok = MessagePackDataF0AB(msg, listStr);
		}
		else if (MSGCMD.CMDF0 == msg.getCmd())
		{
			ok = MessagePackDataF0F0(msg, listStr);
		}

		return ok;
	}

	private boolean MessagePackDataF055(Msg msg, String[] listStr)
	{
		boolean ok = false;
		// String str;
		long val = 0;
		int index = 0;
		int strIndex = 0;
		if (listStr == null)
		{
			return ok;
		}
		if (listStr.length != 1)
		{
			return ok;
		}

		ok = true;
		index = 0;

		byte[] data = new byte[1];
		// for (int i = 0; i < 7; i++)
		// {
		val = StrTools.stringToInt(listStr[strIndex++]);
		val = StrTools.intToBcdInt(val);
		data[index++] = (byte) (val % 256);
		// }

		msg.setData(data);
		msg.setDataLen(index);
		msg.setCheckOk(true);
		// MessageEnCode(msg);
		return ok;
	}

	private boolean MessagePackDataF056(Msg msg, String[] listStr)
	{
		boolean ok = false;
		// String str;
		long val = 0;
		int index = 0;
		int strIndex = 0;
		if (listStr == null)
		{
			return ok;
		}
		if (listStr.length != 1)
		{
			return ok;
		}

		ok = true;
		index = 0;
		byte[] data = new byte[1];
		// for (int i = 0; i < 7; i++)
		{
			val = StrTools.stringToInt(listStr[strIndex++]);
			val = StrTools.intToBcdInt(val);
			data[index++] = (byte) (val % 256);
		}

		msg.setData(data);
		msg.setDataLen(index);
		msg.setCheckOk(true);
		// MessageEnCode(msg);
		return ok;
	}

	private boolean MessagePackDataF057(Msg msg, String[] listStr)
	{
		boolean ok = false;
		// String str;
		long val = 0;
		int index = 0;
		int strIndex = 0;
		if (listStr == null)
		{
			return ok;
		}
		if (listStr.length != 1)
		{
			return ok;
		}

		ok = true;
		index = 0;
		byte[] data = new byte[1];
		// for (int i = 0; i < 7; i++)
		{
			val = StrTools.stringToInt(listStr[strIndex++]);
			val = StrTools.intToBcdInt(val);
			data[index++] = (byte) (val % 256);
		}

		msg.setData(data);
		msg.setDataLen(index);
		msg.setCheckOk(true);
		// MessageEnCode(msg);
		return ok;
	}

	private boolean MessagePackDataF0AA(Msg msg, String[] listStr)
	{
		boolean ok = false;
		// String str;
		long val = 0;
		int index = 0;
		int strIndex = 0;
		if (listStr == null)
		{
			return ok;
		}
		if (listStr.length != 7)
		{
			return ok;
		}

		ok = true;
		index = 0;
		byte[] data = new byte[7];
		for (int i = 0; i < 7; i++)
		{
			val = StrTools.stringToInt(listStr[strIndex++]);
			data[index++] = (byte) (val % 256);
		}

		msg.setData(data);
		msg.setDataLen(index);
		msg.setCheckOk(true);
		// MessageEnCode(msg);
		return ok;
	}

	private boolean MessagePackDataF0AB(Msg msg, String[] listStr)
	{
		boolean ok = false;
		// String str;
		long val = 0;
		int index = 0;
		int strIndex = 0;
		if (listStr == null)
		{
			return ok;
		}
		if (listStr.length != 7)
		{
			return ok;
		}

		ok = true;
		index = 0;
		byte[] data = new byte[7];
		for (int i = 0; i < 7; i++)
		{
			val = StrTools.stringToInt(listStr[strIndex++]);
			data[index++] = (byte) (val % 256);
		}

		msg.setData(data);
		msg.setDataLen(index);
		msg.setCheckOk(true);
		// MessageEnCode(msg);
		return ok;
	}

	private boolean MessagePackDataF0F0(Msg msg, String[] listStr)
	{
		boolean ok = false;
		// String str;
		long val = 0;
		int index = 0;
		int strIndex = 0;
		if (listStr == null)
		{
			return ok;
		}
		if (listStr.length != 1)
		{
			return ok;
		}

		ok = true;
		index = 0;
		byte[] data = new byte[1];
		// for (int i = 0; i < 7; i++)
		{
			val = StrTools.stringToInt(listStr[strIndex++]);
			val = StrTools.intToBcdInt(val);
			data[index++] = (byte) (val % 256);
		}

		msg.setData(data);
		msg.setDataLen(index);
		msg.setCheckOk(true);
		// MessageEnCode(msg);
		return ok;
	}

	private boolean MessagePackDataEF(Msg msg, String[] listStr)
	{
		boolean ok = false;

		if (MSGCMD.CMD00 == msg.getCmd())
		{
			ok = MessagePackDataEF00(msg, listStr);
		}
		else if (MSGCMD.CMD01 == msg.getCmd())
		{
			ok = MessagePackDataEF01(msg, listStr);
		}
		else if (MSGCMD.CMD06 == msg.getCmd())
		{
			ok = MessagePackDataEF06(msg, listStr);
		}
		return ok;
	}

	private boolean MessagePackDataEF00(Msg msg, String[] listStr)
	{
		boolean ok = false;
		// String str;
		long val = 0;
		int index = 0;
		int strIndex = 0;
		if (listStr == null)
		{
			return ok;
		}
		if (listStr.length != 1)
		{
			return ok;
		}

		ok = true;
		index = 0;
		byte[] data = new byte[1];
		// for (int i = 0; i < 7; i++)
		// {
		val = StrTools.stringToInt(listStr[strIndex++]);
		val = StrTools.intToBcdInt(val);
		data[index++] = (byte) (val % 256);
		// }

		msg.setData(data);
		msg.setDataLen(index);
		msg.setCheckOk(true);
		// MessageEnCode(msg);
		return ok;
	}

	private boolean MessagePackDataEF01(Msg msg, String[] listStr)
	{
		boolean ok = false;
		// String str;
		long val = 0;
		int index = 0;
		int strIndex = 0;
		if (listStr == null)
		{
			return ok;
		}
		if (listStr.length != 1)
		{
			return ok;
		}

		ok = true;
		index = 0;
		byte[] data = new byte[1];
		// for (int i = 0; i < 7; i++)
		{
			val = StrTools.stringToInt(listStr[strIndex++]);
			val = StrTools.intToBcdInt(val);
			data[index++] = (byte) (val % 256);
		}

		msg.setData(data);
		msg.setDataLen(index);
		msg.setCheckOk(true);
		// MessageEnCode(msg);
		return ok;
	}

	private boolean MessagePackDataEF06(Msg msg, String[] listStr)
	{
		boolean ok = false;
		// String str;
		long val = 0;
		int index = 0;
		int strIndex = 0;
		if (listStr == null)
		{
			return ok;
		}
		if (listStr.length != 1)
		{
			return ok;
		}

		ok = true;
		index = 0;
		byte[] data = new byte[1];

		val = StrTools.stringToInt(listStr[strIndex++]);
		val = StrTools.intToBcdInt(val);

		msg.setData(data);
		data[index++] = (byte) (val % 256);

		msg.setDataLen(index);
		msg.setCheckOk(true);
		return ok;
	}

	public List<Msg> checkMessage(Buff buff)
	{
		Log.i(TAG, "public List<Msg> checkMessage( byte[] data start......");
		
		List<Msg> listMsg;//
		List<Msg> listResultMsg = new ArrayList<Msg>();
		// byte [] data = buff.data;
		List<byte[]> listData = recvDataToList(buff);
		
		Log.i(TAG, "public List<Msg> checkMessage( byte[] data recvDataToList...... listData:"
		         + listData.size());
		
		Msg msg;
		// boolean ok = false;
		// for()
		for (byte[] da : listData)
		{
			Log.i(TAG, "public List<Msg> checkMessage( byte[] data recvDataToList...... da:"
			         + StrTools.bytesToHexString(da));
			msg = dataToMessage(da);
			Log.i(TAG, "public List<Msg> checkMessage( byte[] data recvDataToList...... msg:"
			         + msg);
			// ok = false;
			listMsg = DeCodeMessage(msg);
			if (listMsg != null)
			{
				Log.i(TAG, "public List<Msg> checkMessage( byte[] data end......Msg"
				         + msg.getId() + ":" + listMsg.size());

				for (Msg msgTmp : listMsg)
				{
					listResultMsg.add(msgTmp);
				}
			}
			else
			{
				Log.i(TAG, "public List<Msg> checkMessage( byte[] data end......Msg"
				         + msg.getId() + ": not result data");

			}
		}
		Log.i(TAG, "public List<Msg> checkMessage( byte[] data end......listResultMsg:"
		         + listResultMsg.size());
		return listResultMsg;
	}

	private List<byte[]> recvDataToList(Buff bu)   // 把接收到的数据一条分解成多条
	{
		Log.i(TAG, "private List<byte[]> recvDataToList( byte[] data start......bu.length:"
		         + bu.length);
		boolean start = false;
		// boolean ok;
		List<byte[]> listData = new ArrayList<byte[]>();
		byte[] da = null;
		byte[] buff;
		int buffLength = 0;
		buff = new byte[bu.data.length];
		int i = 0;
		for (i = 0; i < bu.data.length; i++)
		{
			if (bu.data[i] == 0x55)
			{
				break;
			}

		}
		for (; i < bu.data.length; i++)
		{
			buff[buffLength++] = bu.data[i];
			if (bu.data[i] == 0x55)
			{
				if (start)   // 结束了
				{
					// ok = false;
					// 通过 返回byte【】 ，不通过返回null；
					da = checkStrMsg(buff, buffLength);
					if (da != null)
					{
						listData.add(da);
					}
					buffLength = 0;
					start = false;
				}
				else
				{
					buffLength = 0;
					buff[buffLength++] = bu.data[i];
					start = true;
				}
			}
		}
		// bu. data = new byte[buffLength];
		// data.length = buffLength;
		bu.data = new byte[buffLength];
		bu.length = buffLength;
		System.arraycopy(buff, 0, bu.data, 0, buffLength);// (src, srcPos, dest,
		// destPos, length)

		Log.i(TAG, "bu.length:" + bu.length);
		StrTools.printHexString(bu.data);
		Log.i(TAG, "private List<byte[]> recvDataToList( byte[] data end......listData:"
		         + listData.size());
		return listData;
	}

	// / <summary>
	// / 检查消息是否正确
	// / </summary>
	// / <param name="data"></param>
	// / <param name="len"></param>
	// / <returns></returns>// 通过 返回byte【】 ，不通过返回null；
	private byte[] checkStrMsg(byte[] data, int len)
	{
		Log.i(TAG, " private byte[] checkStrMsg(byte[] data, int len) start......len:"
		         + len);
		byte[] buff;
		byte[] da = null;
		int i = 0;
		int buffLength = 0;
		buff = new byte[len * 2];
		// 把0x54 0x02还原成0x54 把0x54 0x01还原成0x55
		buff[buffLength++] = 0x55;
		for (i = 1; i < len - 1; i++)
		{
			buff[buffLength++] = data[i];
			if (data[i] == 0x54)
			{
				if (data[i + 1] == 0x01)
				{
					buff[buffLength - 1] = 0x55;
					i++;
					continue;
				}
				if (data[i + 1] == 0x02)
				{
					buff[buffLength - 1] = 0x54;
					i++;
					continue;
				}
				buff[buffLength++] = data[i];
			}
		}
		buff[buffLength++] = data[i];
		if (buffLength < 9)
		{
			// ok = false;
			return null;
		}
		short cs = buff[buffLength - 3];
		cs *= 256;
		cs += buff[buffLength - 2];

		// if (new CRC16().checkCrc(buff, buffLength - 3, (short)cs))
		{
			// ok = true;
			da = new byte[buffLength];
			System.arraycopy(buff, 0, da, 0, buffLength);
			Log.i(TAG, " private byte[] checkStrMsg(byte[] data, int len) end......check ok buffLength:"
			         + buffLength);
			return da;
		}

		// Log.i(TAG, " private byte[] checkStrMsg(byte[] data, int len) end......");
		// return null;
	}

	// / <summary>
	// / 把byte[]组装成msg
	// / </summary>
	// / <param name="data"></param>
	// / <returns></returns>
	private Msg dataToMessage(byte[] data)
	{

		Log.i(TAG, " private Msg dataToMessage(byte[] data) start....data:"
		         + StrTools.bytesToHexString(data));
		int len = 0;
		int val = 0;
		int index = 0;
		long value = 0;
		Msg msg = new Msg();
		String torken = "";
		byte[] da;
		// 长度
		len = StrTools.byteToUint(data[2]);
		len *= 256;
		len += StrTools.byteToUint(data[1]);
		msg.setPackLen(len);
		// 命令类别
		msg.setCmdType(MSGCMDTYPE.valueOf(data[3] & 0xFF));
		Log.i(TAG, "data[3]:" + data[3]);
		Log.i(TAG, "msg.getCmdType():" + msg.getCmdType());
		// 命令字
		msg.setCmd(MSGCMD.valueOf(data[4] & 0xFF));
		Log.i(TAG, "data[4]:" + data[4]);
		Log.i(TAG, "msg.getCmd():" + msg.getCmd());
		// 命令序号 data[5][6]
		// 扩展信息 data[7][8]
		// 状态
		msg.setState(MSGSTATE.valueOf(data[9]));
		// 设备ID
		byte[] b = new byte[8];
		b[0] = data[10];
		b[1] = data[11];
		b[2] = data[12];
		b[3] = data[13];
		b[4] = data[14];
		b[5] = data[15];
		b[6] = data[16];
		b[7] = data[17];
		msg.setId(b);

		// Torken
		val = StrTools.byteToUint(data[18]);
		torken = new String(data, 19, val);

		// msg.getId() = Integer.parseInt(msg.getId()+"", 16);
		len -= packLeng + val + 1;
		msg.setDataLen(0);
		if (len > 0)
		{
			msg.setDataLen(len);
			da = new byte[len];
			System.arraycopy(data, packLeng + val - 2, da, 0, len);
			msg.setData(da);
		}

		Log.i(TAG, " private Msg dataToMessage(byte[] data) eng......");
		return msg;
	}

	// / <summary>
	// / 解码
	// / </summary>
	// / <param name="msg"></param>
	// / <returns></returns>
	private List<Msg> DeCodeMessage(Msg msg)
	{
		Log.i(TAG, " private List<Msg> DeCodeMessage(Msg msg) start......msgId:"
		         + msg.getId() + " msgCmdType:" + msg.getCmdType());
		List<Msg> listResultMsg = null;

		DeCodeMessageBfore(msg);
		if (MSGCMDTYPE.CMDTYPE_A0 == msg.getCmdType())
		{
			listResultMsg = DeCodeMessageA0(msg);
		}
		else if (MSGCMDTYPE.CMDTYPE_AA == msg.getCmdType())
		{
			listResultMsg = DeCodeMessageAA(msg);
		}
		else if (MSGCMDTYPE.CMDTYPE_AC == msg.getCmdType())
		{
			listResultMsg = DeCodeMessageAC(msg);
		}
		else if (MSGCMDTYPE.CMDTYPE_F0 == msg.getCmdType())
		{
			listResultMsg = DeCodeMessageF0(msg);
		}
		else if (MSGCMDTYPE.CMDTYPE_EF == msg.getCmdType())
		{
			listResultMsg = DeCodeMessageEF(msg);
		}
		else if (MSGCMDTYPE.CMDTYPE_EF == msg.getCmdType())
		{
			listResultMsg = DeCodeMessageEF(msg);
		}
		else
		{
			Log.i(TAG, " private List<Msg> DeCodeMessage(Msg msg)  msg.getCmdType()error:"
			         + msg.getCmdType());
		}

		DeCodeMessageAfter(msg);

		Log.i(TAG, " private List<Msg> DeCodeMessage(Msg msg) eng......");
		return listResultMsg;
	}

	// / <summary>
	// / 方法之前运行
	// / </summary>
	// / <param name="msg"></param>
	private void DeCodeMessageBfore(Msg msg)
	{
		if (msg.getState() == MSGSTATE.MSG_SEND_OK) // 命令返回的结果，成功。
		{
			msg.setAck(true);
		}
		else if (msg.getState() == MSGSTATE.MSG_SEND_ERROE)  // 命令返回的结果，失败。
		{
			msg.setNoAck(true);

		}
		else   // 请求命令
		{
			msg.setAck(false);
		}
	}

	// / <summary>
	// / 方法之后运行
	// / </summary>
	// / <param name="msg"></param>
	private void DeCodeMessageAfter(Msg msg)
	{
		MessageEnCode(msg);
	}/**/

	private List<Msg> DeCodeMessageA0(Msg msg)
	{
		Log.i(TAG, " private List<Msg> DeCodeMessageA0(Msg msg) start......msg:"
		         + msg.getId());
		List<Msg> listResultMsg = null;
		// ok = false;
		DeCodeMessageA0Bfore(msg);
		// msg.cmd = MSGCMD.CMD01;
		if (MSGCMD.CMD00 == msg.getCmd())
		{
			listResultMsg = DeCodeMessageA000(msg);
		}
		else if (MSGCMD.CMD01 == msg.getCmd())
		{
			listResultMsg = DeCodeMessageA001(msg);
		}

		DeCodeMessageA0After(msg);
		Log.i(TAG, " private List<Msg> DeCodeMessageA0(Msg msg) end......");
		return listResultMsg;
	}

	// / <summary>
	// / 方法之前运行
	// / </summary>
	// / <param name="msg"></param>
	private void DeCodeMessageA0Bfore(Msg msg)
	{
		// if (msg.getState() == MSGSTATE.MSG_SEND_OK) //命令返回的结果，成功。
		// {
		// msg.setAck(true);
		// }
		// else if (msg.getState() == MSGSTATE.MSG_SEND_ERROE)//命令返回的结果，失败。
		// {
		// msg.setNoAck(true);

		// }
		// else //请求命令
		// {
		// msg.setAck(false);
		// }
	}

	// / <summary>
	// / 方法之后运行
	// / </summary>
	// / <param name="msg"></param>
	private void DeCodeMessageA0After(Msg msg)
	{
		// MessageEnCode(msg);
	}

	// / <summary>
	// /
	// / </summary>
	// / <param name="msg"></param>
	// / <param name="ok"></param>
	// / <returns></returns>
	private List<Msg> DeCodeMessageAA(Msg msg)
	{
		List<Msg> listResultMsg = null;
		// ok = false;
		DeCodeMessageAABfore(msg);

		if (MSGCMD.CMD00 == msg.getCmd())
		{
			listResultMsg = DeCodeMessageAA00(msg);
		}
		else if (MSGCMD.CMDEE == msg.getCmd())
		{
			listResultMsg = DeCodeMessageAAEE(msg);
		}
		else if (MSGCMD.CMDFF == msg.getCmd())
		{
			listResultMsg = DeCodeMessageAAFF(msg);
		}
		DeCodeMessageAAAfter(msg);
		return listResultMsg;
	}

	// / <summary>
	// / 方法之前运行
	// / </summary>
	// / <param name="msg"></param>
	private void DeCodeMessageAABfore(Msg msg)
	{

	}

	// / <summary>
	// / 方法之后运行
	// / </summary>
	// / <param name="msg"></param>
	private void DeCodeMessageAAAfter(Msg msg)
	{

	}

	// / <summary>
	// /
	// / </summary>
	// / <param name="msg"></param>
	// / <param name="ok"></param>
	// / <returns></returns>
	private List<Msg> DeCodeMessageAC(Msg msg)
	{
		List<Msg> listResultMsg = null;
		// ok = false;

		Log.i(TAG, " private List<Msg> DeCodeMessageAC(Msg msg) start......msgId:"
		         + msg.getId() + " msgCmdType:" + msg.getCmdType());

		DeCodeMessageACBfore(msg);
		if (MSGCMD.CMD01 == msg.getCmd())
		{
			listResultMsg = DeCodeMessageAC01(msg);
		}
		else if (MSGCMD.CMD02 == msg.getCmd())
		{
			listResultMsg = DeCodeMessageAC02(msg);
		}

		DeCodeMessageACAfter(msg);
		Log.i(TAG, " private List<Msg> DeCodeMessageAC(Msg msg) end......");

		return listResultMsg;
	}

	// / <summary>
	// / 方法之前运行
	// / </summary>
	// / <param name="msg"></param>
	private void DeCodeMessageACBfore(Msg msg)
	{

	}

	// / <summary>
	// / 方法之后运行
	// / </summary>
	// / <param name="msg"></param>
	private void DeCodeMessageACAfter(Msg msg)
	{

	}

	// / <summary>
	// /
	// / </summary>
	// / <param name="msg"></param>
	// / <param name="ok"></param>
	// / <returns></returns>
	private List<Msg> DeCodeMessageF0(Msg msg)
	{
		List<Msg> listResultMsg = null;
		// ok = false;
		DeCodeMessageF0Bfore(msg);

		if (MSGCMD.CMD55 == msg.getCmd())
		{
			listResultMsg = DeCodeMessageF055(msg);
		}
		else if (MSGCMD.CMD56 == msg.getCmd())
		{
			listResultMsg = DeCodeMessageF056(msg);
		}
		else if (MSGCMD.CMD57 == msg.getCmd())
		{
			listResultMsg = DeCodeMessageF057(msg);
		}
		else if (MSGCMD.CMDAA == msg.getCmd())
		{
			listResultMsg = DeCodeMessageF0AA(msg);
		}
		else if (MSGCMD.CMDAB == msg.getCmd())
		{
			listResultMsg = DeCodeMessageF0AB(msg);
		}
		else if (MSGCMD.CMDF0 == msg.getCmd())
		{
			listResultMsg = DeCodeMessageF0F0(msg);
		}
		DeCodeMessageF0After(msg);
		return listResultMsg;
	}

	// / <summary>
	// / 方法之前运行
	// / </summary>
	// / <param name="msg"></param>
	private void DeCodeMessageF0Bfore(Msg msg)
	{

	}

	// / <summary>
	// / 方法之后运行
	// / </summary>
	// / <param name="msg"></param>
	private void DeCodeMessageF0After(Msg msg)
	{

	}

	// / <summary>
	// /
	// / </summary>
	// / <param name="msg"></param>
	// / <param name="ok"></param>
	// / <returns></returns>
	private List<Msg> DeCodeMessageEF(Msg msg)
	{
		List<Msg> listResultMsg = null;
		// ok = false;
		DeCodeMessageEFBfore(msg);

		if (MSGCMD.CMD00 == msg.getCmd())
		{
			listResultMsg = DeCodeMessageEF00(msg);
		}
		else if (MSGCMD.CMD01 == msg.getCmd())
		{
			listResultMsg = DeCodeMessageEF01(msg);
		}
		else if (MSGCMD.CMD06 == msg.getCmd())
		{
			listResultMsg = DeCodeMessageEF06(msg);
		}
		else if (MSGCMD.CMD07 == msg.getCmd())
		{
			listResultMsg = DeCodeMessageEF07(msg);
		}
		// switch (msg.cmd)
		// {
		// case MSGCMD.CMD01: listResultMsg = DeCodeMessage0901(msg, ok); break;
		// case MSGCMD.CMD02: listResultMsg = DeCodeMessage0902(msg, ok); break;
		// case MSGCMD.CMD03: listResultMsg = DeCodeMessage0903(msg, ok); break;
		// case MSGCMD.CMD04: listResultMsg = DeCodeMessage0904(msg, ok); break;
		// case MSGCMD.CMD05: listResultMsg = DeCodeMessage0905(msg, ok); break;
		//
		// default: break;
		// }
		DeCodeMessageEFAfter(msg);
		return listResultMsg;
	}

	// / <summary>
	// / 方法之前运行
	// / </summary>
	// / <param name="msg"></param>
	private void DeCodeMessageEFBfore(Msg msg)
	{

	}

	// / <summary>
	// / 方法之后运行
	// / </summary>
	// / <param name="msg"></param>
	private void DeCodeMessageEFAfter(Msg msg)
	{

	}

	// / <summary>
	// / 方法之前运行
	// / </summary>
	// / <param name="msg"></param>
	private void DeCodeMessage0ABfore(Msg msg)
	{

	}

	// / <summary>
	// / 方法之后运行
	// / </summary>
	// / <param name="msg"></param>
	private void DeCodeMessage0AAfter(Msg msg)
	{

	}

	// / <summary>
	// / 命令 A0 00
	// / </summary>
	// / <param name="msg"></param>
	// / <param name="ok"></param>
	// / <returns></returns>
	private List<Msg> DeCodeMessageA000(Msg msg)
	{
		// ok = true;
		Log.i(TAG, " private List<Msg> DeCodeMessageA000(Msg msg) start......msg:"
		         + msg.getId());
		List<Msg> listResultMsg = new ArrayList<Msg>();
		if (msg.getState() == MSGSTATE.MSG_SEND_OK)   // 命令返回的结果，成功或失败。
		{
			// if (MessageEnCode(msg))
			{
				Log.i(TAG, " private List<Msg> DeCodeMessageA000(Msg msg)  execute ok......");
				Cfg.tcpTorken = msg.getData();
				listResultMsg.add(msg);
			}
		}
		else   // 请求命令
		{

		}
		Log.i(TAG, " private List<Msg> DeCodeMessageA000(Msg msg) end......");
		return listResultMsg;
	}

	// / <summary>
	// / 命令 03 02
	// / </summary>
	// / <param name="msg"></param>
	// / <param name="ok"></param>
	// / <returns></returns>
	private List<Msg> DeCodeMessageA001(Msg msg)
	{
		// ok = true;
		Log.i(TAG, " private List<Msg> DeCodeMessageA001(Msg msg) ......msg:"
		         + msg.getId());
		List<Msg> listResultMsg = new ArrayList<Msg>();
		if ((msg.getState() == MSGSTATE.MSG_SEND_OK)
		        || (msg.getState() == MSGSTATE.MSG_SEND_ERROE))   // 命令返回的结果，成功或失败。
		{
			// if (MessageEnCode(msg))
			{
				listResultMsg.add(msg);
			}
		}
		else   // 请求命令
		{
			msg.setSponse(true);
			listResultMsg.add(msg);

		}
		return listResultMsg;
	}

	// / <summary>
	// / 命令 04 01 设置控制器时段信息（0x0001）
	// / </summary>
	// / <param name="msg"></param>
	// / <param name="ok"></param>
	// / <returns></returns>
	private List<Msg> DeCodeMessageAA00(Msg msg)
	{

		List<Msg> listResultMsg = new ArrayList<Msg>();
		if ((msg.getState() == MSGSTATE.MSG_SEND_OK)
		        || (msg.getState() == MSGSTATE.MSG_SEND_ERROE))
		{
			if (msg.getState() == MSGSTATE.MSG_SEND_OK)
			{
				if (msg.getDataLen() == 1)
				{
					if ((msg.getData() != null) && (msg.getData().length >= 1))
					{

					}
				}
			}
			// 命令返回的结果，成功或失败。
			if (MessageEnCode(msg))
			{
				listResultMsg.add(msg);
			}
		}
		else   // 请求命令
		{

		}
		return listResultMsg;
	}

	// / <summary>
	// / 命令 04 02
	// / </summary>
	// / <param name="msg"></param>
	// / <param name="ok"></param>
	// / <returns></returns>
	private List<Msg> DeCodeMessageAAEE(Msg msg)
	{
		// ok = true;

		Log.i(TAG, "DeCodeMessageAAEE");
		List<Msg> listResultMsg = new ArrayList<Msg>();
		
		if ((msg.getState() == MSGSTATE.MSG_SEND)
			|| (msg.getState() == MSGSTATE.MSG_SEND_OK))   // 命令返回的结果，成功或失败。
		{
			String id = StrTools.byteHexNumToStr(msg.getId());
			Dev dev = Cfg.getDevById(id);
			if (dev == null)
			{
				return listResultMsg;
			}
			
			byte[] data = msg.getData();//获取到body数据
			Log.i(TAG, "Body:" + new String(data));
		}
		else   // 请求命令
		{

		}
		return listResultMsg;
	}

	// / <summary>
	// / 命令 04 03
	// / </summary>
	// / <param name="msg"></param>
	// / <param name="ok"></param>
	// / <returns></returns>
	private List<Msg> DeCodeMessageAAFF(Msg msg)
	{
		// ok = true;
		List<Msg> listResultMsg = new ArrayList<Msg>();
		if ((msg.getState() == MSGSTATE.MSG_SEND_OK))   // 命令返回的结果，成功或失败。
		{
			String id = StrTools.byteHexNumToStr(msg.getId());
			Dev dev = Cfg.getDevById(id);
			if (dev == null)
			{
				return listResultMsg;
			}
			byte[] data = msg.getData();
			if ((data == null) || (data.length <= 5))
			{
				return listResultMsg;
			}
			String[] info = new String(data).split(":");
			if (info.length != 2)
			{
				if ((info[0].equals("LIGHT")) && (!info[1].equals("?")))
				{
					if (info[1].equals("1"))  // 开灯
					{
						dev.setLightState(true);
					}
					else if (info[1].equals("0"))    // 关灯
					{
						dev.setLightState(false);
					}
					else
					{

					}

				}

				if ((info[0].equals("TEMP")) && (!info[1].equals("?")))
				{
					dev.setTemp(Integer.parseInt(info[1]));
				}
				if (info[0].equals("LIGHTR"))
				{
					dev.setLampRVal(Integer.parseInt(info[1]));
				}
				if (info[0].equals("LIGHTG"))
				{
					dev.setLampGVal(Integer.parseInt(info[1]));
				}
				if (info[0].equals("LIGHTB"))
				{
					dev.setLampBVal(Integer.parseInt(info[1]));
				}
			}
			// if (MessageEnCode(msg)) {
			// listResultMsg.add(msg);
			// }
		}
		else   // 请求命令
		{

		}
		return listResultMsg;
	}

	// / <summary>
	// / 命令 06 02
	// / </summary>
	// / <param name="msg"></param>
	// / <param name="ok"></param>
	// / <returns></returns>
	private List<Msg> DeCodeMessageAC01(Msg msg)
	{
		// ok = true;
		int value = 0;
		int index = 0;
		int dataIndex = 0;
		List<Msg> listResultMsg = new ArrayList<Msg>();
		if ((msg.getState() == MSGSTATE.MSG_SEND_OK)
		        || (msg.getState() == MSGSTATE.MSG_SEND_ERROE))
		{
			if (msg.getState() == MSGSTATE.MSG_SEND_OK)
			{
				if (msg.getDataLen() == 15)
				{
					if ((msg.getData() != null) && (msg.getData().length >= 15))
					{
						String[] strList = new String[5];
						byte[] data = msg.getData();
						index = 0;
						dataIndex = 0;
						// DHCP
						value = StrTools.byteToUint(data[dataIndex++]);
						strList[index++] = value + "";
						// IP地址
						strList[index++] = IpTools.getIpV4StringByByte(data,
						                   dataIndex);
						dataIndex += 4;
						// 子网掩码
						strList[index++] = IpTools.getIpV4StringByByte(data,
						                   dataIndex);
						dataIndex += 4;
						// 默认网关
						strList[index++] = IpTools.getIpV4StringByByte(data,
						                   dataIndex);
						dataIndex += 4;
						// 端口号
						value = StrTools.byteToUint(data[dataIndex++]) * 256;
						value += StrTools.byteToUint(data[dataIndex++]);
						strList[index++] = value + "";
						msg.setResult(strList);
					}
				}
			}
			// 命令返回的结果，成功或失败。
			if (MessageEnCode(msg))
			{
				listResultMsg.add(msg);
			}
		}
		else   // 请求命令
		{

		}
		return listResultMsg;
	}

	/**
	 * DeCodeMessageAC02
	 *
	 * @param msg
	 * @return
	 */
	private List<Msg> DeCodeMessageAC02(Msg msg)
	{
		// ok = true;
		int value = 0;
		int index = 0;
		int dataIndex = 0;
		List<Msg> listResultMsg = new ArrayList<Msg>();
		if ((msg.getState() == MSGSTATE.MSG_SEND_OK)
		        || (msg.getState() == MSGSTATE.MSG_SEND_ERROE))
		{
			if (msg.getState() == MSGSTATE.MSG_SEND_OK)
			{
				if (msg.getDataLen() == 28)
				{
					if ((msg.getData() != null) && (msg.getData().length >= 28))
					{
						String[] strList = new String[6];
						byte[] data = msg.getData();
						index = 0;
						dataIndex = 0;
						// 主服务器IP地址
						strList[index++] = IpTools.getIpV4StringByByte(data,
						                   dataIndex);
						dataIndex += 4;
						// 主服务器端口
						value = StrTools.byteToUint(data[dataIndex++]) * 256;
						value += StrTools.byteToUint(data[dataIndex++]);
						strList[index++] = value + "";
						// 主服务器密码(不用)
						dataIndex += 8;
						strList[index++] = "";
						// 备用服务器IP地址
						strList[index++] = IpTools.getIpV4StringByByte(data,
						                   dataIndex);
						dataIndex += 4;
						// 备用服务器端口
						value = StrTools.byteToUint(data[dataIndex++]) * 256;
						value += StrTools.byteToUint(data[dataIndex++]);
						strList[index++] = value + "";
						// 备用服务器密码(不用)
						strList[index++] = "";
						msg.setResult(strList);
					}
				}
			}
			// 命令返回的结果，成功或失败。
			if (MessageEnCode(msg))
			{
				listResultMsg.add(msg);
			}
		}
		else   // 请求命令
		{

		}
		return listResultMsg;
	}

	/**
	 * DeCodeMessageF055
	 *
	 * @param msg
	 * @return
	 */
	private List<Msg> DeCodeMessageF055(Msg msg)
	{
		// ok = true;
		long value = 0;
		// int index = 0;
		// int dataIndex = 0;
		List<Msg> listResultMsg = new ArrayList<Msg>();
		if ((msg.getState() == MSGSTATE.MSG_SEND_OK)
		        || (msg.getState() == MSGSTATE.MSG_SEND_ERROE))
		{
			if (msg.getState() == MSGSTATE.MSG_SEND_OK)
			{
				if (msg.getDataLen() == 169)
				{
					if ((msg.getData() != null)
					        && (msg.getData().length >= 169))
					{
						String[] strList = new String[169];

						for (int i = 0; i < 169; i++)
						{
							value = StrTools.byteToUint(msg.getData()[i]);
							value = StrTools.bcdIntToInt(value);
							strList[i] = value + "";
							if (i % 7 == 0)
							{
								Log.i(TAG, "");
							}
							else
							{
								Log.i(TAG, value + "\t\t\t    ");
							}
						}
						Log.i(TAG, "");
						msg.setResult(strList);
					}
				}
			}
			// 命令返回的结果，成功或失败。
			if (MessageEnCode(msg))
			{
				listResultMsg.add(msg);
			}
		}
		else   // 请求命令
		{

		}
		return listResultMsg;
	}

	/**
	 * DeCodeMessageF056
	 *
	 * @param msg
	 * @return
	 */
	private List<Msg> DeCodeMessageF056(Msg msg)
	{
		// ok = true;
		long value = 0;
		// int index = 0;
		// int dataIndex = 0;
		List<Msg> listResultMsg = new ArrayList<Msg>();
		if ((msg.getState() == MSGSTATE.MSG_SEND_OK)
		        || (msg.getState() == MSGSTATE.MSG_SEND_ERROE))
		{
			if (msg.getState() == MSGSTATE.MSG_SEND_OK)
			{
				if (msg.getDataLen() == 833)
				{
					if ((msg.getData() != null)
					        && (msg.getData().length >= 833))
					{
						String[] strList = new String[833];

						for (int i = 0; i < 833; i++)
						{
							value = StrTools.byteToUint(msg.getData()[i]);
							value = StrTools.bcdIntToInt(value);
							strList[i] = value + "";
						}
						msg.setResult(strList);
					}
				}
			}
			// 命令返回的结果，成功或失败。
			if (MessageEnCode(msg))
			{
				listResultMsg.add(msg);
			}
		}
		else   // 请求命令
		{

		}
		return listResultMsg;
	}

	/**
	 * DeCodeMessageF057
	 *
	 * @param msg
	 * @return
	 */
	private List<Msg> DeCodeMessageF057(Msg msg)
	{
		// ok = true;
		long value = 0;
		// int index = 0;
		// int dataIndex = 0;
		List<Msg> listResultMsg = new ArrayList<Msg>();
		if ((msg.getState() == MSGSTATE.MSG_SEND_OK)
		        || (msg.getState() == MSGSTATE.MSG_SEND_ERROE))
		{
			if (msg.getState() == MSGSTATE.MSG_SEND_OK)
			{
				if (msg.getDataLen() == 259)
				{
					if ((msg.getData() != null)
					        && (msg.getData().length >= 259))
					{
						String[] strList = new String[259];

						for (int i = 0; i < 259; i++)
						{
							value = StrTools.byteToUint(msg.getData()[i]);
							if (i == 0)
							{
								value = StrTools.bcdIntToInt(value);
							}
							strList[i] = value + "";
						}
						msg.setResult(strList);
					}
				}
			}
			// 命令返回的结果，成功或失败。
			if (MessageEnCode(msg))
			{
				listResultMsg.add(msg);
			}
		}
		else   // 请求命令
		{

		}
		return listResultMsg;
	}

	// / <summary>
	// / 命令 07 04
	// / </summary>
	// / <param name="msg"></param>
	// / <param name="ok"></param>
	// / <returns></returns>
	private List<Msg> DeCodeMessageF0AA(Msg msg)
	{
		// ok = true;
		long value = 0;
		// int index = 0;
		// int dataIndex = 0;
		List<Msg> listResultMsg = new ArrayList<Msg>();
		if ((msg.getState() == MSGSTATE.MSG_SEND_OK)
		        || (msg.getState() == MSGSTATE.MSG_SEND_ERROE))
		{
			if (msg.getState() == MSGSTATE.MSG_SEND_OK)
			{
				if (msg.getDataLen() == 120)
				{
					if ((msg.getData() != null)
					        && (msg.getData().length >= 120))
					{
						String[] strList = new String[120];

						for (int i = 0; i < 120; i++)
						{
							value = StrTools.byteToUint(msg.getData()[i]);
							value = StrTools.bcdIntToInt(value);
							strList[i] = value + "";
						}
						msg.setResult(strList);
					}
				}
			}
			// 命令返回的结果，成功或失败。
			if (MessageEnCode(msg))
			{
				listResultMsg.add(msg);
			}
		}
		else   // 请求命令
		{

		}
		return listResultMsg;
	}

	// / <summary>
	// / 命令 07 05
	// / </summary>
	// / <param name="msg"></param>
	// / <param name="ok"></param>
	// / <returns></returns>
	private List<Msg> DeCodeMessageF0AB(Msg msg)
	{
		// ok = true;
		List<Msg> listResultMsg = new ArrayList<Msg>();
		if ((msg.getState() == MSGSTATE.MSG_SEND_OK)
		        || (msg.getState() == MSGSTATE.MSG_SEND_ERROE))
		{
			if (msg.getState() == MSGSTATE.MSG_SEND_OK)
			{
				if (msg.getDataLen() == 7)
				{
					if ((msg.getData() != null) && (msg.getData().length >= 7))
					{
						String[] strList = new String[7];
						for (int i = 0; i < 7; i++)
						{
							strList[i] = ((int) msg.getData()[i]) + "";
						}
						msg.setResult(strList);
					}
				}
			}
			// 命令返回的结果，成功或失败。
			if (MessageEnCode(msg))
			{
				listResultMsg.add(msg);
			}
		}
		else   // 请求命令
		{

		}
		return listResultMsg;
	}

	// / <summary>
	// / 命令 07 06
	// / </summary>
	// / <param name="msg"></param>
	// / <param name="ok"></param>
	// / <returns></returns>
	private List<Msg> DeCodeMessageF0F0(Msg msg)
	{
		// ok = true;
		int value = 0;
		// int index = 0;
		// int dataIndex = 0;
		List<Msg> listResultMsg = new ArrayList<Msg>();
		if ((msg.getState() == MSGSTATE.MSG_SEND_OK)
		        || (msg.getState() == MSGSTATE.MSG_SEND_ERROE))
		{
			if (msg.getState() == MSGSTATE.MSG_SEND_OK)
			{
				if (msg.getDataLen() == 56)
				{
					if ((msg.getData() != null) && (msg.getData().length >= 56))
					{
						String[] strList = new String[56];

						for (int i = 0; i < 56; i++)
						{
							value = StrTools.byteToUint(msg.getData()[i]);
							strList[i] = value + "";
						}
						msg.setResult(strList);
					}
				}
			}
			// 命令返回的结果，成功或失败。
			if (MessageEnCode(msg))
			{
				listResultMsg.add(msg);
			}
		}
		else   // 请求命令
		{

		}
		return listResultMsg;
	}

	// / <summary>
	// / 命令 09 01
	// / </summary>
	// / <param name="msg"></param>
	// / <param name="ok"></param>
	// / <returns></returns>
	private List<Msg> DeCodeMessageEF00(Msg msg)
	{
		// ok = true;
		List<Msg> listResultMsg = new ArrayList<Msg>();
		if ((msg.getState() == MSGSTATE.MSG_SEND_OK)
		        || (msg.getState() == MSGSTATE.MSG_SEND_ERROE))   // 命令返回的结果，成功或失败。
		{
			if (MessageEnCode(msg))
			{
				listResultMsg.add(msg);
				if (msg.getState() == MSGSTATE.MSG_SEND_OK)
				{
					Log.i(TAG, DateTools.getNowTimeString() + "==> socket登录成功！");
				}
				else if (msg.getState() == MSGSTATE.MSG_SEND_ERROE)
				{
					Log.i(TAG, DateTools.getNowTimeString() + "==>socket登录失败！");
				}
			}
		}
		else   // 请求命令
		{

		}
		return listResultMsg;
	}

	// / <summary>
	// / 命令 09 02
	// / </summary>
	// / <param name="msg"></param>
	// / <param name="ok"></param>
	// / <returns></returns>
	private List<Msg> DeCodeMessageEF01(Msg msg)
	{
		// ok = true;
		List<Msg> listResultMsg = new ArrayList<Msg>();
		if ((msg.getState() == MSGSTATE.MSG_SEND_OK)
		        || (msg.getState() == MSGSTATE.MSG_SEND_ERROE))   // 命令返回的结果，成功或失败。
		{
			if (MessageEnCode(msg))
			{
				listResultMsg.add(msg);
				if (msg.getState() == MSGSTATE.MSG_SEND_OK)
				{
					Log.i(TAG, DateTools.getNowTimeString() + "==> socket心跳成功！");
				}
				else if (msg.getState() == MSGSTATE.MSG_SEND_ERROE)
				{
					Log.i(TAG, DateTools.getNowTimeString() + "==>socket心跳失败！");
				}
			}
		}
		else   // 请求命令
		{
			msg.setState(MSGSTATE.MSG_SEND_OK);
			msg.setSponse(true);
		}
		return listResultMsg;
	}

	/**
	 *
	 * @param msg
	 * @return
	 */
	private List<Msg> DeCodeMessageEF06(Msg msg)
	{
		List<Msg> listResultMsg = new ArrayList<Msg>();
		if (msg.getState() == MSGSTATE.MSG_SEND_OK)   // 命令返回的结果，成功或失败。
		{
			Cfg.isSubmitDev = true;
		}
		else   // 请求命令
		{

		}
		return listResultMsg;
	}

	private List<Msg> DeCodeMessageEF07(Msg msg)
	{
		List<Msg> listResultMsg = new ArrayList<Msg>();
		if (msg.getState() == MSGSTATE.MSG_SEND_OK)   // 命令返回的结果，成功或失败。
		{
			Cfg.isDeleteDev = true;
		}
		else   // 请求命令
		{

		}
		return listResultMsg;
	}

}
