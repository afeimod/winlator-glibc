package com.winlator.midi;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.concurrent.Executors;

import cn.sherlock.com.sun.media.sound.SF2Soundbank;
import cn.sherlock.com.sun.media.sound.SoftSynthesizer;
import jp.kshoji.javax.sound.midi.Receiver;
import jp.kshoji.javax.sound.midi.ShortMessage;

public class MidiHandler {
    private static final String TAG = "MidiHandler";
    private DatagramSocket socket;
    private boolean running = false;
    private static final short SERVER_PORT = 7942;
    private static final short CLIENT_PORT = 7941;
    private static final int BUF_SIZE = 9;
    private final ByteBuffer receiveData = ByteBuffer.allocate(BUF_SIZE).order(ByteOrder.LITTLE_ENDIAN);
    private final DatagramPacket receivePacket = new DatagramPacket(receiveData.array(), BUF_SIZE);
    private SoftSynthesizer synth;
    private Receiver recv;

    public void setSoundBank(SF2Soundbank soundBank) {
        if (synth != null)
            synth.close();

        try {
            synth = new SoftSynthesizer();
            synth.open();
            synth.loadAllInstruments(soundBank);
            recv = synth.getReceiver();
        } catch (Exception e) {}
    }
    public void start() {
        running = true;
        Executors.newSingleThreadExecutor().execute(() -> {
            try {
                socket = new DatagramSocket(null);
                socket.setReuseAddress(true);
                socket.bind(new InetSocketAddress((InetAddress) null, SERVER_PORT));

                while (running) {
                    socket.receive(receivePacket);
                    receiveData.rewind();
                    handleRequest(receiveData);
                }
            } catch (IOException e) {
            }
        });
    }

    public void stop() {
        running = false;

        if (socket != null) {
            socket.close();
            socket = null;
        }
    }

    private void handleRequest(ByteBuffer received) {
        byte requestCode = received.get();
        switch (requestCode) {
            case RequestCodes.MIDI_SHORT:
                try {
                    recv.send(new ShortMessage(received.get(), received.get(), received.get()), -1);
                } catch (Exception e) {
                    e.printStackTrace();
                }
                break;
            case RequestCodes.MIDI_LONG:
                break;
        }
    }
}
