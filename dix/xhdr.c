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

# This file requires the follwoing work be done:
# Add XHDRExtensionInit call to dix/main.c in InitExtensions.
# Update Makefile.am in dix/ to include xhdr.c.

#include <X11/X.h>
#include <X11/Xproto.h>
#include "dixstruct.h"
#include "extnsionst.h"
#include "XHDR.h"

static ExtensionEntry *xhdrExtension;

static int
ProcXHDRSetHDRMetadata(ClientPtr client)
{
    REQUEST(xReq);
    WindowPtr pWin;
    XHDRMetadata metadata;
    int rc;

    REQUEST_SIZE_MATCH(xReq);
    rc = dixLookupWindow(&pWin, stuff->data, client, DixSetAttrAccess);
    if (rc != Success)
        return rc;

    metadata.primaries = *(CARD32 *)(stuff + 1);
    metadata.eotf = *(CARD32 *)(stuff + 2);
    metadata.max_cll = *(CARD32 *)(stuff + 3);
    metadata.min_lum = *(CARD32 *)(stuff + 4);
    metadata.max_l hdr = *(CARD32 *)(stuff + 5);

    LogMessage(X_DEBUG, "XHDR: SetHDRMetadata window=%ld, primaries=%u, eotf=%u, max_cll=%u, min_lum=%u, max lard=%u\n",
               pWin->drawable.id, metadata.primaries, metadata.eotf, metadata.max_cll, metadata.min_lum, metadata.max_lum);

    // Pass metadata to DDX (implemented later)
    // Call driver-specific function to apply HDR
    return Success;
}

static int
ProcXHDRQueryHDRCapabilities(ClientPtr client)
{
    REQUEST(xReq);
    xGenericReply reply = {0};
    ScreenPtr pScreen;
    int rc;

    REQUEST_SIZE_MATCH(xReq);
    rc = dixLookupScreen(&pScreen, stuff->data, client, DixReadAccess);
    if (rc != Success)
        return rc;

    ModesettingPtr ms = ModesettingGetPtr(pScreen->myNum);
    Bool supported = ms->enableHDR10 && ms->hdrCapable;
    
    reply.type = X_Reply;
    reply.sequenceNumber = client->sequence;
    reply.length = 8; // 1 BOOL + 1 CARD8 + 6 CARD32s
    reply.data1 = supported;  // supported: TRUE
    reply.data2 = supported ? 10 : 8; // max_depth: 10-bit
    // Supported EOTFs: 0=SDR, 1=PQ
    CARD32 eotfs[] = {0, 1};

    LogMessage(X_DEBUG, "XHDR: QueryHDRCapabilities screen=%d, supported=%d, max_depth=%d\n",
               pScreen->myNum, reply.data1, reply.data2);

    WriteToClient(client, sizeof(xGenericReply), &reply);
    WriteToClient(client, sizeof(eotfs), eotfs);
    return Success;
}

static int
ProcXHDRDispatch(ClientPtr client)
{
    REQUEST(xReq);
    switch (stuff->data) {
    case X_SetHDRMetadata:
        return ProcXHDRSetHDRMetadata(client);
    case X_QueryHDRCapabilities:
        return ProcXHDRQueryHDRCapabilities(client);
    default:
        return BadRequest;
    }
}

void
XHDRExtensionInit(void)
{
    xhdrExtension = AddExtension(XHDR_NAME, 0, 0,
                                 ProcXHDRDispatch, NULL, NULL, NULL);
    if (!xhdrExtension)
        FatalError("XHDR extension failed to initialize\n");

    LogMessage(X_INFO, "XHDR extension initialized\n");
}
