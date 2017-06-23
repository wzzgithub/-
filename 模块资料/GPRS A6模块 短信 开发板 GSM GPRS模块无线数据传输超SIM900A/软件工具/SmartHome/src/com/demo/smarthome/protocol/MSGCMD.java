package com.demo.smarthome.protocol;

public enum MSGCMD {

	CMD01((byte) 0x01), CMD02((byte) 0x02), CMD03((byte) 0x03), CMD04(
			(byte) 0x04), CMD05((byte) 0x05), CMD06((byte) 0x06), CMD07(
			(byte) 0x07), CMD08((byte) 0x08), CMD09((byte) 0x09), CMD0A(
			(byte) 0x0A), CMD0B((byte) 0x0B), CMD0C((byte) 0x0C), CMD10(
			(byte) 0x10), CMD11((byte) 0x11), CMD12((byte) 0x12), CMD13(
			(byte) 0x13), CMD14((byte) 0x14), CMD55((byte) 0x55), CMD56(
			(byte) 0x56), CMD57((byte) 0x57), CMDAA((byte) 0xAA), CMDAB(
			(byte) 0xAB), CMDF0((byte) 0xF0), CMDEE((byte) 0xEE), CMDFF(
			(byte) 0xFF), CMD00((byte) 0x0);
	private byte val;

	// 构造方法
	private MSGCMD(byte val) {
		this.val = val;
	} // get set 方法

	public static MSGCMD valueOf(int val) {
		switch (val % 0xFF) {
		case 0x00:
			return CMD00;
		case 0x01:
			return CMD01;
		case 0x02:
			return CMD02;
		case 0x03:
			return CMD03;
		case 0x04:
			return CMD04;
		case 0x05:
			return CMD05;
		case 0x06:
			return CMD06;
		case 0x07:
			return CMD07;
		case 0x08:
			return CMD08;
		case 0x09:
			return CMD09;
		case 0x0A:
			return CMD0A;
		case 0x0B:
			return CMD0B;
		case 0x0C:
			return CMD0C;
		case 0x10:
			return CMD10;
		case 0x11:
			return CMD11;
		case 0x12:
			return CMD12;
		case 0x13:
			return CMD13;
		case 0x14:
			return CMD14;
		case 0x55:
			return CMD55;
		case 0x56:
			return CMD56;
		case 0x57:
			return CMD57;
		case -86:
		case 0xAA:
			return CMDAA;
		case -85:
		case 0xAB:
			return CMDAB;
		case -16:
		case 0xF0:
			return CMDF0;
		case -18:
		case 0xEE:
			return CMDEE;
		case -1:
		case 0xFF:
			return CMDFF;
		default:
			return null;
		}
	}

	public byte val() {
		return (byte) val;
	}
	// // get set 方法
	// public int getVal() {
	// return val;
	// }
	//
	// public void setVal(int val) {
	// this.val = val;
	// }

}