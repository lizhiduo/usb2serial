package com.example.lizd.syno_qrcode;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.view.menu.ExpandedMenuView;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.ccb.dev.interfaces.Qrcode;

import java.io.UnsupportedEncodingException;

public class MainActivity extends AppCompatActivity implements View.OnClickListener{

    final static String TAG = "syno_dev_qrcode";

    private boolean isrun = false;
    private TextView tv;
    private TextView state_tv;
    //private Button read_btn;
    String rdata;
    private EditText time_ed;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        tv = (TextView) findViewById(R.id.tv);
        state_tv = (TextView) findViewById(R.id.state_tv);
        //read_btn = (Button) findViewById(R.id.read);
        time_ed = (EditText) findViewById(R.id.ed_time);

        Qrcode.set_port_speed(115200);
        Qrcode.set_coded_format("UTF-8");
        Qrcode.open_qrcode_dev();

//        Toast.makeText(this,"time is too long", Toast.LENGTH_LONG).show();
    }

    @Override
    public void onClick(View view) {

        switch (view.getId()){
            case R.id.read :
                if(!isrun){
                    tv.setText("read:");
                    new MyThread().start();
                }else{
                    Log.d(TAG, "Thread is runing...");
                    Toast.makeText(this, "is reading, please wait...", Toast.LENGTH_LONG).show();
                }
            break;

            case R.id.state_tv:
                int ret = Qrcode.touch_qrcode_dev();
//                Log.d(TAG,"ret: "+ret);
                state_tv.setText("getstate :" + ret);
                break;

            case R.id.tv:
                tv.setText("read:");
                break;
        }
    }

    private class MyThread extends Thread{
            @Override
            public void run() {
                //to do
                isrun = true;
                int wait_time = 1;


                 if(time_ed.getText().length()  != 0){
                    String str = time_ed.getText().toString();
                     wait_time = Integer.parseInt(str);
                     if(wait_time > 25500){
                         wait_time = 25500;
                         Log.d(TAG, "time is too long, set time=25500");
                     }
                }else{
                     wait_time = -1;
                 }

                Log.d(TAG, "waittime: "+ wait_time);


                rdata = Qrcode.read_qrcode_dev(wait_time); //ms

                if(rdata != null){

//                    try {
//                        byte b[] = rdata.getBytes("GBK");//GBK  unicode  UTF-8
//                        for(int i=0; i<b.length; i++){
//                            int tmp = b[i] & 0xff;
//                            Log.d(TAG, "i : "+ i + " hex: " + Integer.toHexString(tmp) + " data: "+ b[i] );
//                        }
//                    }catch (Exception e){
//                        Log.d(TAG, ""+e);
//                    }

                    Log.d(TAG,"lenth: "+ rdata.length() + "  rdata : " + rdata);
                }else{
                    Log.d(TAG,"rdata is null");
                }

                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if(rdata != null){
                            tv.setText("lenth: " + rdata.length() + "\n" + "read: " + rdata);
                        }else {
                            tv.setText("read: " + rdata);
                        }
                    }
                });

                isrun = false;
            }
        }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Qrcode.close_qrcode_dev();
    }


    public static String toUtf8(String str) {
        String result = null;
        try {
            result = new String(str.getBytes("gbk"), "UTF-8");  //gbk，unicode，utf-8
        } catch (UnsupportedEncodingException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return result;
    }

    // 字符串转换成Unicode
    public static String string2Unicode(String string) {
        StringBuffer unicode = new StringBuffer();
        for (int i = 0; i < string.length(); i++) {
            char c = string.charAt(i);
            unicode.append("\\u" + Integer.toHexString(c));

            Log.d(TAG,""+ string.charAt(i));
        }
        return unicode.toString();
    }
}
