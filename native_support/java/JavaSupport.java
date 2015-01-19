
package com.android.godot;

import java.lang.Class;
import java.lang.Object;
import java.lang.String;
import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;

public class JavaSupport {
    static String signature(Class clazz) {
        String type = clazz.toString();
        if (type.matches("^class .+")) {
            if (type.matches("^class \\["))
                return type.replaceFirst("^class ", "").replace(".", "/");
            else
                return ("L"+type.replaceFirst("^class ", "")+";").replace(".", "/");
        }else {
            if (type.equals("boolean")) {
                return "Z";
            }else if (type.equals("int")) {
                return "I";
            }else if (type.equals("byte")) {
                return "B";
            }else if (type.equals("char")) {
                return "C";
            }else if (type.equals("double")) {
                return "D";
            }else if (type.equals("float")) {
                return "F";
            }else if (type.equals("long")) {
                return "L";
            }else if (type.equals("void")) {
                return "V";
            }
        }
        return "";
    }

    static Class[] clazzes(String sig) {
        if (sig.equals("I")) {
            return new Class[]{int.class, byte.class, char.class, long.class};
        }else if (sig.equals("Z")) {
            return new Class[]{boolean.class};
        }else if (sig.equals("F")) {
            return new Class[]{float.class, double.class};
        }else if (sig.equals("V")) {
            return new Class[]{void.class};
        }else {
            try {
                return new Class[]{Class.forName(sig)};
            }catch(Exception e) {
                return new Class[]{};
            }
        }
    }

    public static String constructorSig(String className) throws Exception {
        Class cls = Class.forName(className);
        if (cls.getConstructors().length > 0) {
            Constructor cons = cls.getConstructors()[0];
            Class[] clazzes = cons.getParameterTypes();
            String sig = "(";
            for (Class cl :clazzes) {
                sig += signature(cl);
            }
            sig += ")V";
            return sig;
        }
        return "";
    }

    public static String constructorSig(String className, String[] params) {
        try {
            Class cls = Class.forName(className);
            Constructor[] conses = cls.getConstructors();
            for (Constructor cons :conses) {
                Class[] p_types = cons.getParameterTypes();

                if (p_types.length == params.length) {
                    String sig = "(";
                    boolean failed = true;
                    for (int n = 0, t = p_types.length; n < t; n++) {
                        Class[] clsColl = clazzes(params[n]);
                        for (Class _testCls :clsColl){
                            if (_testCls.equals(p_types[n])) {
                                failed = false;
                                sig += signature(_testCls);
                            }
                        }
                    }
                    if (failed) {
                        continue;
                    }
                    sig += ")V";
                    return  sig;
                }
            }
        }catch(Exception e) {
            android.util.Log.v("gen_debug", "error : "+ e.getMessage());
            return "";
        }
        return "";
    }

    public static String objectSig(Object obj) {
        return signature(obj.getClass());
    }

    public static String objectSig2(Object obj) {
        return obj.getClass().toString().replaceFirst("^class +", "");
    }

    public static String memberSig(Object obj, String memberName) {
        Class cls = obj.getClass();
        try {
            Field field = cls.getField(memberName);
            return signature(field.getType());
        }catch (Exception e) {
            System.out.print(e);
            return "";
        }
    }

    public static String methodSig(Object obj, String methodName) {
        Class cls = obj.getClass();

        Method[] ms = cls.getMethods();
        for(Method m :ms) {
            if (m.getName().equals(methodName)) {
                String sig = "(";
                Class[] p_cls = m.getParameterTypes();
                for (Class clazz :p_cls) {
                    sig+=signature(clazz);
                }
                sig += ")";
                sig += signature(m.getReturnType());
                return sig;
            }
        }
        return "";
    }

    public static String methodSig(Object obj, boolean isStatic, String methodName, String[] params) {
        Class cls;
        if (isStatic) {
            cls = (Class)obj;
        }else {
            cls = obj.getClass();
        }

        Method[] ms = cls.getMethods();
        for(Method m :ms) {
            if (m.getName().equals(methodName)) {
                Class[] p_types = m.getParameterTypes();
                if (p_types.length == params.length) {
                    String sig = "(";
                    boolean failed = false;
                    for (int n = 0, t = p_types.length; n < t; n++) {
                        Class[] clsColl = clazzes(params[n]);
                        boolean win = false;
                        for (Class _testCls :clsColl){
                            if (_testCls.equals(p_types[n])) {
                                win = true;
                                sig += signature(_testCls);
                                break;
                            }
                        }
                        if (!win) {
                            failed = true;
                            break;
                        }
                    }
                    if (failed || Modifier.isStatic(m.getModifiers()) != isStatic) {
                        continue;
                    }
                    sig += ")";
                    sig += signature(m.getReturnType());
                    return sig;
                }
            }
        }
        return "";
    }

    public static void emitSignal(Object sender, String signal, Object... params) {
        String[] signatures = new String[params.length];
        for (int n = 0, t = params.length; n < t; n++) {
            signatures[n] = signature(params[n].getClass());
        }
        cEmitSignal(sender, signal, signatures, params);
    }

    public static void addUserSignal(Object sender, String signal, String... params) {
        cAddUserSignal(sender, signal, params);
    }

    public static native void cEmitSignal(Object sender, String signal, String[] signatures, Object[] params);
    public static native void cAddUserSignal(Object sender, String signal, String[] params);
}