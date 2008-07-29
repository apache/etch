java_package etch.examples.benchmark.thrift
cpp_namespace etch.examples.benchmark.thrift

service Perf
{
  i32 add( i32 x, i32 y )
  
  i32 sum( list<i32> values )
  
  async void report( i32 code, string msg )
}
