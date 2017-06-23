package com.demo.smarthome.protocol;

import com.demo.smarthome.iprotocol.IMsg;
import com.demo.smarthome.iprotocol.IProtocol;
import com.demo.smarthome.tools.StrTools;

public class Msg implements IMsg {
	public Msg() {
		this.state = MSGSTATE.MSG_SEND;
	}

	public Msg(byte[] devId, int cmdType, int cmd) {
		this.id = devId;
		this.cmdType = MSGCMDTYPE.valueOf(cmdType);
		this.cmd = MSGCMD.valueOf(cmd);
		this.state = MSGSTATE.MSG_SEND;
		// MSGCMD m = MSGCMD.CMD0A;
	}

	public Msg(byte[] devId, int cmdType, int cmd, int state) {
		this.id = devId;
		this.cmdType = MSGCMDTYPE.valueOf(cmdType);
		this.cmd = MSGCMD.valueOf(cmd);
		this.state = MSGSTATE.valueOf(state);
		// MSGCMD m = MSGCMD.CMD0A;
	}

	public Msg(byte[] devId, MSGCMDTYPE cmdType, MSGCMD cmd) {
		this.id = devId;
		this.cmdType = cmdType;
		this.cmd = cmd;
		this.state = MSGSTATE.MSG_SEND;
		// MSGCMD m = MSGCMD.CMD0A;
	}

	public Msg(byte[] devId, MSGCMDTYPE cmdType, MSGCMD cmd, MSGSTATE state) {
		this.id = devId;
		this.cmdType = cmdType;
		this.cmd = cmd;
		this.state = state;
		// MSGCMD m = MSGCMD.CMD0A;
	}

	private byte[] id = new byte[0];// 16进制MAC地址(8Byte),设备号,不足前面补 0
	private int packLen;
	private int dataLen;
	private MSGCMDTYPE cmdType = MSGCMDTYPE.CMDTYPE_A0; // 命令类别
	private MSGCMD cmd = MSGCMD.CMD00; // 命令
	private boolean checkOk; // 正确的报文
	private boolean isACK; // 确认回答
	private boolean isNoACK; // 否认回答
	private boolean isSponse; // 是否需要回复
	private MSGSTATE state;
	private short crc;
	private byte[] data;
	private byte[] deCode; // 接收后去掉译码的编码
	private byte[] enCode; // 发送前没有转译的编码
	private byte[] recvData; // 接收到的数据
	private byte[] sendData; // 要发送的数据
	private String[] result; // 开始可以存储传递的参数 ，根据cmdTYpe 和cmd 解析的字符串结果。

	private byte[] torken = new byte[0];// 通信令牌，登录成功后才有此字段,第一字节表示长度
	private int info = 0; // 2个字节 扩展信息 包括序号，加密等其它扩展

	public byte[] getId() {
		return id;
	}

	public void setId(byte[] id) {
		this.id = id;
	}

	public byte[] getTorken() {
		return torken;
	}

	public void setTorken(String torken) {
		this.torken = StrTools.hexStringToBytes(torken);
	}

	public void setTorken(byte[] torken) {
		this.torken = torken;
	}

	public int getInfo() {
		return info;
	}

	public void setInfo(int info) {
		this.info = info;
	}

	public int getPackLen() {
		return packLen;
	}

	public void setPackLen(int packLen) {
		this.packLen = packLen;
	}

	public int getDataLen() {
		return dataLen;
	}

	public void setDataLen(int dataLen) {
		this.dataLen = dataLen;
	}

	public MSGCMDTYPE getCmdType() {
		return cmdType;
	}

	public void setCmdType(MSGCMDTYPE cmdType) {
		this.cmdType = cmdType;
	}

	public MSGCMD getCmd() {
		return cmd;
	}

	public void setCmd(MSGCMD cmd) {
		this.cmd = cmd;
	}

	public boolean isCheckOk() {
		return checkOk;
	}

	public void setCheckOk(boolean checkOk) {
		this.checkOk = checkOk;
	}

	public boolean isACK() {
		return isACK;
	}

	public void setACK(boolean isACK) {
		this.isACK = isACK;
	}

	public boolean isNoACK() {
		return isNoACK;
	}

	public void setNoACK(boolean isNoACK) {
		this.isNoACK = isNoACK;
	}

	public boolean isSponse() {
		return isSponse;
	}

	public void setSponse(boolean isSponse) {
		this.isSponse = isSponse;
	}

	public MSGSTATE getState() {
		return state;
	}

	public void setState(MSGSTATE state) {
		this.state = state;
	}

	public short getCrc() {
		return crc;
	}

	public void setCrc(short crc) {
		this.crc = crc;
	}

	public byte[] getData() {
		return data;
	}

	public void setData(byte[] data) {
		this.data = data;
	}

	public byte[] getDeCode() {
		return deCode;
	}

	public void setDeCode(byte[] deCode) {
		this.deCode = deCode;
	}

	public byte[] getEnCode() {
		return enCode;
	}

	public void setEnCode(byte[] enCode) {
		this.enCode = enCode;
	}

	public byte[] getRecvData() {
		return recvData;
	}

	public void setRecvData(byte[] recvData) {
		this.recvData = recvData;
	}

	public byte[] getSendData() {
		return sendData;
	}

	public void setSendData(byte[] sendData) {
		this.sendData = sendData;
	}

	public String[] getResult() {
		return result;
	}

	public void setResult(String[] result) {
		this.result = result;
	}

	public IProtocol getProtocol() {
		return protocol;
	}

	public void setProtocol(IProtocol protocol) {
		this.protocol = protocol;
	}

	// / <summary>
	// / 消息是不是登录成功标示
	// / </summary>
	// / <returns></returns>
	public boolean msgTypeIsLoginOk() {
		boolean ok = false;
		if ((this.cmdType == MSGCMDTYPE.CMDTYPE_A0)
				&& (this.cmd == MSGCMD.CMD00)
				&& (this.state == MSGSTATE.MSG_SEND_OK)) {
			ok = true;
		}
		return ok;
	}

	// / <summary>
	// / 是不是指定命令的确认回复
	// / </summary>
	// / <param name="cmdType"></param>
	// / <param name="cmd"></param>
	// / <returns></returns>
	public boolean msgIsACKByCmdType(MSGCMDTYPE cmdType, MSGCMD cmd) {
		boolean ok = false;
		if ((this.cmdType == cmdType) && (this.cmd == cmd) && (isACK)) {
			ok = true;
		}
		return ok;

	}

	// /***
	// *
	// * @param cmdType
	// * @param cmd
	// * @param id
	// * @return
	// */
	// public boolean msgIsACKByCmdType(MSGCMDTYPE cmdType, MSGCMD cmd,int id) {
	// boolean ok = false;
	// if ((this.cmdType == cmdType) && (this.cmd == cmd) && (isACK)) {
	// if(this.data == null){
	// return ok;
	// }
	// if(data.length<1){
	// return ok;
	// }
	// if( data[0] == id){
	// ok = true;
	// }
	// }
	// return ok;
	//
	// }

	// / <summary>
	// / 指定命令的确认回复和主动提交
	// / </summary>
	// / <param name="cmdType"></param>
	// / <param name="cmd"></param>
	// / <returns></returns>
	public boolean msgIsSendAndACKByCmdType(int cmdType, int cmd) {
		boolean ok = false;
		if ((this.cmdType == MSGCMDTYPE.valueOf(cmdType))
				&& (this.cmd == MSGCMD.valueOf(cmd))
				&& (isACK || (state == MSGSTATE.MSG_SEND))) {
			ok = true;
		}
		return ok;

	}

	public boolean msgIsSendAndACKByCmdType(MSGCMDTYPE cmdType, MSGCMD cmd) {
		boolean ok = false;
		if ((this.cmdType == cmdType) && (this.cmd == cmd)
				&& (isACK || (state == MSGSTATE.MSG_SEND))) {
			ok = true;
		}
		return ok;

	}

	public boolean msgStateTypeIsSend() {
		boolean ok = false;
		if (this.state == MSGSTATE.MSG_SEND_OK) {
			ok = true;
		}
		return ok;
	}

	public void setAck(boolean ack) {
		this.isACK = ack;
		this.isNoACK = false;
	}

	public void setNoAck(boolean noAck) {
		this.isNoACK = noAck;
		this.isACK = false;
	}

	@Override
	public String toString() {
		// TODO Auto-generated method stub
		String str = "";
		str += "		id:" + id;
		str += "		packLen:" + packLen;
		str += "		dataLen:" + dataLen;
		str += "		cmdType:" + cmdType;
		str += "		cmd:" + cmd;
		str += "		checkOk:" + checkOk;
		str += "		isACK:" + isACK;
		str += "		isNoACK:" + isNoACK;
		str += "		isSponse:" + isSponse;
		str += "		state:" + state;
		str += "		crc:" + crc;
		str += "		data:" + StrTools.bytesToHexString(data);
		str += "		deCode:" + StrTools.bytesToHexString(deCode);
		str += "		enCode:" + StrTools.bytesToHexString(enCode);
		str += "		recvData:" + StrTools.bytesToHexString(recvData);
		str += "		sendData:" + StrTools.bytesToHexString(sendData);
		int i = 1;
		if (result != null) {
			for (String s : result) {
				str += "  " + i++ + " : " + s;
			}
		}
		// str += "		result:"+result;
		return str;
	}

	// ====================private====================
	private IProtocol protocol = null;

}
