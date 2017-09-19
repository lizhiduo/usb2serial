package com.ccb.dev.interfaces;

/**
 * Created by lizd on 2017/9/13.
 */

public class Qrcode {

    static {
        System.loadLibrary("syno_dev_ops");
    }

    /**
     *
     * @return
     *  0 - 成功
     *  其它 - 失败
     */

    public static native int open_qrcode_dev();

    /**
     *
     * @return
     *  0 - 成功
     *  其它 - 失败
     */
    public static native int close_qrcode_dev();

    /**
     *
     * @param waitTime
     * @return
     */
    public static native String read_qrcode_dev(int waitTime);

    /**
     *
     * @return
     */
    public static native int touch_qrcode_dev();
}
