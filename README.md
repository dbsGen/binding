# binding
Binding GDScript to native languages.

# Usage

For Java:

      var JClass = NF.find("com.android.godot.TestObj")
      var test_obj = JClass.new(22)
      print(test_obj.a)
      
For Object-C:

	  var MyObject = NF.find("MyObject")
	  var test_obj = MyObject.new()
	  obj.b = 998
	  print(obj.b) # obj.b() for method.

Signal in Java:

      JavaSupport.addUserSignal(this, "event_test2", "str");
      JavaSupport.emitSignal(this, "event_test2", "this is test");

