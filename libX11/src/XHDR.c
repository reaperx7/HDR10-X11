#BSD 3-Clause License
#
#Copyright (c) 2025, James Powell
#
#Redistribution and use in source and binary forms, with or without
#modification, are permitted provided that the following conditions are met:
#
#1. Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
#2. Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
#3. Neither the name of the copyright holder nor the names of its
#   contributors may be used to endorse or promote products #derived from
#   this software without specific prior written permission.
#
#THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
#FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
#DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
#SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
#CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
#OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <X11/Xlib.h>
#include <X11/extensions/XHDR.h>

Status
XSetHDRMetadata(Display *dpy, Window win, XHDRMetadata *metadata)
{
    xReq req;
    xReq *ptr;
    int length = sizeof(xReq) + sizeof(XHDRMetadata);

    LockDisplay(dpy);
    GetReqExtra(XHDR, SetHDRMetadata, sizeof(XHDRMetadata), ptr);
    ptr->data = win;
    memcpy((char *)(ptr + 1), metadata, sizeof(XHDRMetadata));
    UnlockDisplay(dpy);
    SyncHandle();
    return Success;
}

Status
XQueryHDRCapabilities(Display *dpy, int screen, Bool *supported, int *max_depth, CARD32 **eotfs, int *num_eotfs)
{
    xGenericReply reply;
    xReq *ptr;

    LockDisplay(dpy);
    GetReq(XHDR, QueryHDRCapabilities, ptr);
    ptr->data = screen;
    if (!_XReply(dpy, (xReply *)&reply, 8 >> 2, False)) {
        UnlockDisplay(dpy);
        return BadRequest;
    }
    *supported = reply.data1;
    *max_depth = reply.data2;
    *num_eotfs = reply.length - 2;
    *eotfs = malloc(*num_eotfs * sizeof(CARD32));
    _XRead(dpy, (char *)*eotfs, *num_eotfs * sizeof(CARD32));
    UnlockDisplay(dpy);
    return Success;
}
