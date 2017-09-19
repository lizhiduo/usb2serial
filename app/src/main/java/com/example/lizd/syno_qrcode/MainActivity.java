package com.example.lizd.syno_qrcode;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.view.menu.ExpandedMenuView;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.ccb.dev.interfaces.Qrcode;

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

        Qrcode.open_qrcode_dev();

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
                }else{
                     wait_time = -1;
                 }

                Log.d(TAG, "waittime: "+ wait_time);


                rdata = Qrcode.read_qrcode_dev(wait_time); //ms

                if(rdata != null){
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
}
