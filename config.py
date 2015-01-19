def can_build(platform):
    return True

def configure(env):
  if env['platform'] == 'android':
        # will copy this to the java folder
        env.android_module_file("java/JavaSupport.java")
        env.android_module_file("java/TestObj.java")