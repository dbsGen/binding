# binding
Binding GDScript to native languages.

# Usage

For Java:

      var JClass = NF.find("com.android.godot.TestObj")
      print(JClass.s_str()) #static method
      var test_obj = JClass.new(22)
      print(test_obj.a)
      
For Object-C:

	  var MyObject = NF.find("MyObject")
	  print(MyObject.getOutput("hello")) #class method
	  var test_obj = MyObject.new()
	  obj.b = 998
	  print(obj.b) # obj.b() for method.

Signal in Java:

      JavaSupport.addUserSignal(this, "event_test2", "str");
      JavaSupport.emitSignal(this, "event_test2", "event from java");

Signal in Object-C:

	  [self addUserSignal:"event_test" withParams:@[@"param"]];
	  [self emitSignal:"event_test" withParams:@[@"event from object-c"]];
