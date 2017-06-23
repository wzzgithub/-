package com.demo.smarthome.protocol;

public enum MSGCMDTYPE {

	CMDTYPE_A0((byte) 0xA0), CMDTYPE_AA((byte) 0xAA), CMDTYPE_AC((byte) 0xAC), CMDTYPE_F0(
			(byte) 0xF0), CMDTYPE_EF((byte) 0xEF);
	private byte val;

	// 构造方法
	private MSGCMDTYPE(byte val) {
		this.val = val;
	}

	public static MSGCMDTYPE valueOf(int val) {
		MSGCMDTYPE type = null;
		System.out.println("val%0xFF:" + val % 0xFF);
		switch (val % 0xFF) {
		case -96:
		case 0xA0:
			type = CMDTYPE_A0;
			break;

		case -86:
		case 0xAA:
			type = CMDTYPE_AA;
			break;
		case -84:
		case 0xAC:
			type = CMDTYPE_AC;
			break;

		case -16:
		case 0xF0:
			type = CMDTYPE_F0;
			break;
		case -17:
		case 0xEF:
			type = CMDTYPE_EF;
			break;

		default:
			break;
		}
		return type;
	}

	// get set 方法
	public byte val() {
		return (byte) val;
	}
	//
	// public void setName(int val) {
	// this.val = val;
	// }

}
