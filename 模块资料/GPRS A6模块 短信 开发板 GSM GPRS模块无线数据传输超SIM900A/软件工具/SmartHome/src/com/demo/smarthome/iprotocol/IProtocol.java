package com.demo.smarthome.iprotocol;

import java.util.List;

import com.demo.smarthome.protocol.Buff;
import com.demo.smarthome.protocol.Msg;

public interface IProtocol {
	List<Msg> checkMessage(Buff buff);

	boolean MessageEnCode(Msg msg);

	boolean MessagePackData(Msg msg, String[] listStr);
}
