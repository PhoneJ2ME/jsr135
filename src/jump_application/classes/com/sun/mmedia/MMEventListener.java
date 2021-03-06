/*
 *
 *  Copyright  1990-2006 Sun Microsystems, Inc. All Rights Reserved.
 *  DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 *  
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License version
 *  2 only, as published by the Free Software Foundation. 
 *  
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  General Public License version 2 for more details (a copy is
 *  included at /legal/license.txt). 
 *  
 *  You should have received a copy of the GNU General Public License
 *  version 2 along with this work; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 *  02110-1301 USA 
 *  
 *  Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
 *  Clara, CA 95054 or visit www.sun.com if you need additional
 *  information or have any questions. 
 */
package com.sun.mmedia;
import java.io.*;
import javax.microedition.media.*;
import com.sun.jump.message.*;
import com.sun.jump.common.*;
import com.sun.jump.isolate.jvmprocess.*;
import com.sun.jumpimpl.module.multimedia.*;

/**
 * Event listener for events delivered from native layer
 */
class MMEventListener implements JUMPMessageHandler {
    /**
     * Executive process for isolate registration as file system listener.
     */
    private static JUMPProcessProxy executive;

    /**
     * Current isolate process which will send messages to executive.
     */
    private static JUMPIsolateProcess thisProcess;

    /**
     * Message dispatcher for registration of handlers.
     */
    private static JUMPMessageDispatcher msgDisp;

    /**
     * Message handler registration token.
     */
    private Object msgToken = null;

    public MMEventListener() {
        if (thisProcess == null) {
            thisProcess = JUMPIsolateProcess.getInstance();
            executive = thisProcess.getExecutiveProcess();
            // Register message handler.
            msgDisp = thisProcess.getMessageDispatcher();
            try {
                msgToken = msgDisp.registerHandler(MultimediaModuleImpl.MESSAGE_TYPE, this);
            } catch (JUMPMessageDispatcherTypeException e) {
                throw new RuntimeException("Cannot register multimedia message handler: "
                    + e.getMessage());
            } catch (IOException e) {
                throw new RuntimeException("Cannot register multimedia message handler: "
                    + e.getMessage());
            }
        }
    }

    /**
     * Performs clean up upon object destruction.
     * Removes current isolate from the list of isolates interested in
     * receiving mount/unmount events.
     */
    protected void finalize() {
        // Unregister message handler.
        if (msgToken != null) {
            try {
                msgDisp.cancelRegistration(msgToken);
            } catch (IOException e) {
                System.err.println("Could not unregister file system message handler: "
                    + e.getMessage());
            }
        }
    }
    /**
     * Handles Multimedia event messages.
     *
     * @param message message from JUMP executive.
     */
    public void handleMessage(JUMPMessage message) {
	    JUMPMessageReader r = new JUMPMessageReader(message);
        String id = r.getUTF();
        String[] args = r.getUTFArray();
        byte[] data = r.getByteArray();
        MMNativeEventImpl nEvent = new MMNativeEventImpl();

        parseNativeEvent(data, nEvent);
        HighLevelPlayer p = HighLevelPlayer.get(nEvent.playerId);
        switch(nEvent.eventId) {
            case MMNativeEventImpl.EOM_EVENT:
                p.sendEvent(PlayerListener.END_OF_MEDIA, new Long(nEvent.value));
                break;
            case MMNativeEventImpl.RSL_EVENT:
                p.sendEvent(PlayerListener.RECORD_STOPPED, new Long(nEvent.value));
                break;
        }
    }

    private native static boolean parseNativeEvent(byte[] data, MMNativeEventImpl event);

}

class MMNativeEventImpl {
    static final int EOM_EVENT = 1;
    static final int RSL_EVENT = 2;
    int playerId;
    int eventId;
    long value;
    void MMNativeEventImpl() {
        playerId = -1;
        eventId = -1;
        value = -1;
    }
}

