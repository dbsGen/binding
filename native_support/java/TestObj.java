
package com.android.godot;

public class TestObj {
    public int a = 1;
    public String b = "nihao";
    public float c = 1.2f;
    public static String d = "static method";

    public TestObj(int set_a) {
        a = set_a;
        JavaSupport.addUserSignal(this, "event_test");
        JavaSupport.addUserSignal(this, "event_test2", "e");
    }

    public int get_a() {
        JavaSupport.emitSignal(this, "event_test");
        return a;
    }

    public String get_str(String input) {
        JavaSupport.emitSignal(this, "event_test2", "this is test");
        return input;
    }

    public static String s_str() {
        return "niii";
    }
}