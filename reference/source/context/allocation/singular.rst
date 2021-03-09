#######
context
#######
**********
allocation
**********
========
singular
========
-----
basic
-----

By default contexts are singular meaning that a single instance exists relative to the parent of the context.
Functions are allocated on the stack and thus have no allocation in the EG logical address space.  All other contexts are allocated within the EG logical address space either as a singular instance beneath their parent or using the specified domain size.  In the following example the SingularObject is an object that exists beneath the root ( which is always singular ) and is itself a singular object.  The example illustrates how when then object is allocated a second time how the system returns a null reference and generates an error in the event log however the system defensively will allow usage of the invalid reference where internally it actually still points to a valid object.

--------
advanced
--------

This section contains more advanced stuff....

SubTitle
^^^^^^^^
Always note how something is something

=====  =====  =======
A      B      A and B
=====  =====  =======
False  False  False
True   False  False
False  True   False
True   True   True
=====  =====  =======


