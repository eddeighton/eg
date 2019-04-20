
//root
template<>
EGCoroutine __interface_root< void >::operator()()
{
  x();

x( 1 );

  co_return;
}
