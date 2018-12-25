package com.ob.jnicallback;

import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
//    private Native mNative = new Native();
    private TextView tv;
    private int callBackCount = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        tv = (TextView) findViewById(R.id.sample_text);
//        tv.setText(stringFromJNI());
//        mNative.nativeInitilize();
//        mNative.nativeThreadStart();
        nativeInitilize();
        nativeThreadStart();
    }

    final static int CANSHU = 1;

    public Handler mHandler = new Handler() {
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case CANSHU:
                    tv.setText("call back count = "+ String.valueOf(callBackCount));
                    break;
                default:
                    break;
            }
        }
    };


    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    @Override
    protected void onDestroy() {
        super.onDestroy();
//        mNative.NativeThreadStop();
        NativeThreadStop();
    }

    public native void nativeInitilize();

    public native void nativeThreadStart();
    public native void NativeThreadStop();

    //this function is callback by native code
    public void onNativeCallback( int count ) {
        Log.d("Native","onNativeCallback count="+count);
        callBackCount = count;
        Message message = new Message();
        message.what = MainActivity.CANSHU;
        mHandler.sendMessage(message);//用activity中的handler发送消息

    }
}
