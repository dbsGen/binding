# binding
Binding GDScript to native languages.

# Usage

GDSCript:

      var JClass = NF.java("com.android.godot.TestObj")
      var test_obj = JClass.new(22)
      print(test_obj.a)
      
Signal in Java:

      JavaSupport.addUserSignal(this, "event_test2", "str");
      JavaSupport.emitSignal(this, "event_test2", "this is test");
