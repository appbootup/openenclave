# Explicit Enclave Opt-in to System OCalls

System OCalls are OCalls that are part of Open Enclave core libraries. These are generally not
called by the enclave developer but instead are called by other functions internal to OE SDK.

Because information from inside the enclave may be transmitted to the untrusted host
during an OCalls, an enclave developer should be able to opt-out of these OCalls
being made. They should also have visibility into what OCalls might be called by OE SDK.

## EDL installation

In order to support ocall opt-in, system EDL files will now be installed as part of the SDK at
`${install_path}/include/openenclave/edl/`. This allows them to be imported into user edl by the
enclave developer.

## Developer opt-in

In order to use any system OCall, it must be imported into the enclave's EDL file. If it is not
included, but the OCall is still called by the enclave application or some part of the SDK,
enclave linking will fail.

Users can import a system OCall the same way any other EDL file would be imported. For example,
if an enclave with EDL file `sample.edl` wants to use the function `oe_log_ocall()` they can do
one of the following:

*Note: These examples assume that `${install_path}/include` is passed to edger8r as an EDL search path*

1. Import just the required OCall

```
// sample.edl
enclave
{
    from "openenclave/edl/logging.edl" import oe_log_ocall;

    trusted
    {
        public void my_sample_ecall();
    };

    untrusted
    {
        public void my_sample_ocall();
    };
}
```

2. Import all OCalls from `tee.edl`

```
// sample.edl
enclave
{
    from "openenclave/edl/logging.edl" import *;

    trusted
    {
        public void my_sample_ecall();
    };

    untrusted
    {
        public void my_sample_ocall();
    };
}
```

In example (1), linking would fail if the enclave made a call to `oe_realloc_ocall()`
(also part of tee.edl) unless the developer modified the import line to
`from "openenclave/edl/tee.edl" import oe_log_ocall, oe_realloc_ocall;`. In this scenario,
example (2) will work without modification.

## EDL local structures

Under this design, system EDL files will prefer using local structures; that is, structures
that are defined in EDL. This is distinct from foreign structures, which are defined in
C/C++ header files and included in the EDL file.

In the case that a foreign structure must be used, system EDL files may only include headers
that are installed with the SDK.

Local EDL structures provide additional security benefits detailed in the
[Full EDL Serialzation design doc](
https://github.com/openenclave/openenclave/blob/master/docs/DesignDocs/full_edl_serialization.md#motivation)
so foreign structures should be avoided whenever possible.

## Developer opt-out

Because system OCalls are generally called by the SDK implementation, developers
may not have direct control over which OCalls will be called by their application.
A developer can refuse to import a function in EDL which will cause a linker error
if it is used, but they should also be able to take action to resolve this failure.

To this end, any OE SDK feature which makes an OCall on behalf of an enclave
should provide a mechanism to disable the behavior which makes an OCall.

For example, the switchless calls feature makes an OCall
`oe_wake_switchless_worker_ocall()`. If there is code compiled into the enclave
which calls this function, the enclave developer will be forced to import that
function into their own EDL. To allow the developer to opt out of this OCall,
it should be possible for the enclave developer to turn off the switchless calls
feature such that an enclave which does not use switchless calls will not require
this OCall. This mechanism should not require the developer to recompile the OE
SDK.

## Exceptions to EDL import and opt-in/out mechanisms

A small number of OCalls in OE SDK are not implemented in EDL. Until these are
converted to EDL, these OCalls will always be included and the developer cannot
opt out of them.

The following 4 OCalls can be implemented in EDL, they just aren't today. Issue
`#2008` tracks converting these to EDL.

* `OE_OCALL_MALLOC`
* `OE_OCALL_FREE`
* `OE_OCALL_THREAD_WAIT`
* `OE_OCALL_THREAD_WAKE`

## List of system OCalls

All system OCalls (other than the previously mentioned exceptions) are found
in one of the following EDL files

* `common/logging.edl`
* `common/memory.edl`
* `common/epoll.edl`
* `common/fcntl.edl`
* `common/ioctl.edl`
* `common/poll.edl`
* `common/signal.edl`
* `common/socket.edl`
* `common/time.edl`
* `common/unistd.edl`
* `common/utsname.edl`
* `common/syscall.edl`
* `common/syscall.edl`
* `common/sgx/sgx.edl`
* `common/sgx/switchless.edl`
