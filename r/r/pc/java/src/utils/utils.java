class utils {    
    static final String DEL_PACKET = "dlpkt";
    static final String DEL_CHANNEL = "dlcha";
    static final String PACKET_NUM = "pnum";
    static final String CHANNEL_NUM = "chan";
    static final String POLL = "pol";
    static final String DIR_POS = "pos";
    static final String DIR_NEG = "neg";
    static final String DIRECTION = "dir";
    static final String CURRENT = "data";
    static final String DELAY = "wait";
    static final String SEND_PACKETS = "send";
    static final String SEND_GLOBAL_RESET = "reset";
    static final String SET_GLOBAL = "glob";
    static final String CONNECT = "conn";
    static final String DISCONNECT = "exit";
    static final String CLEAR_ALL_PACKETS = "clrpks";
    static final String PREVIEW_PACKETS = "prev";

    static final int maxCurrentRes = 4095;
    static final int OFF_CurrentRes = 4095;

    static final int NCHANNELS = 6;
    
    static final boolean DEBUG = true;

    static byte[] intToByteArray_LE(Integer val) {
        byte[] ret = new byte[Integer.SIZE/Byte.SIZE];
        int pow = (int)(Math.pow(2, Byte.SIZE));
        for (int i = 0; i < ret.length; i++) {
            ret[i] = val.byteValue();
            if (val > 0) {
                val = val >>> Byte.SIZE;
                if (val < 0) val = 0;
            }
        }
        return ret;
    }
    static byte[] intToByteArray_BE(Integer val) {
        byte[] ret = new byte[Integer.SIZE/Byte.SIZE];
        int pow = (int)(Math.pow(2, Byte.SIZE));
        for (int i = ret.length-1; i >= 0; i--) {
            ret[i] = val.byteValue();
            if (val > 0) {
                val = val >>> Byte.SIZE;
                if (val < 0) val = 0;
            }
        }
        return ret;
    }

    static void assertErr(boolean cond, String errmsg) {
        if (!cond) {
            System.out.println(errmsg);
        }
    }

    static void assertFail(boolean cond, String errmsg) {
        if (!cond) {
            System.out.println(errmsg);
            System.out.println("fatal.\n");
            System.exit(-1);
        }
    }
 
    static void exceptMsg (String msg, Exception e) {
        if (DEBUG)
            System.out.println(msg + e.getMessage() + "\n");
    }

}
