/*
 * 
 * Copyright  1990-2007 Sun Microsystems, Inc. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 only, as published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included at /legal/license.txt).
 * 
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 * 
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
 * Clara, CA 95054 or visit www.sun.com if you need additional
 * information or have any questions.
 */

#include "KNICommon.h"
#include "jsrop_exceptions.h"

/*  private native int nSetLocator ( int handle , String locator ) ; */
KNIEXPORT KNI_RETURNTYPE_INT
KNIDECL(com_sun_mmedia_DirectRecord_nSetLocator) {
    jint handle = KNI_GetParameterAsInt(1);
    KNIPlayerInfo* pKniInfo = (KNIPlayerInfo*)handle;
    jint locatorLength;
    jchar* locator = NULL;
    jint returnValue = 0;
    javacall_result ret;

    KNI_StartHandles(1);
    KNI_DeclareHandle(locatorHandle);
    KNI_GetParameterAsObject(2, locatorHandle);
    
    locatorLength = KNI_GetStringLength(locatorHandle);
    locator = MMP_MALLOC(locatorLength * sizeof(jchar));
    if (locator) {
        KNI_GetStringRegion(locatorHandle, 0, locatorLength, locator);
        if (pKniInfo && pKniInfo->pNativeHandle) {
            ret = javacall_media_recording_handled_by_native(pKniInfo->pNativeHandle, locator, locatorLength);
            if (JAVACALL_INVALID_ARGUMENT == ret) {
                returnValue = -1;
                REPORT_ERROR1(LC_MMAPI, "[kni_record] Set recording location \
                    return JAVACALL_INVALID_ARGUMENT handle=%d\n", pKniInfo->pNativeHandle);
            } else {
                if (JAVACALL_OK == ret) {
                    pKniInfo->recordState = RECORD_INIT;
                }
                returnValue = (JAVACALL_OK == ret ? 1 : 0);
            }
        }
        MMP_FREE(locator);
    } else {
        REPORT_ERROR1(LC_MMAPI, "[kni_record] nSetLocator memory alloc fail handle=%d\n", pKniInfo->pNativeHandle);
    }

    KNI_EndHandles();
    KNI_ReturnInt(returnValue);
}

/*  private native int nStart ( int handle ) ; */
KNIEXPORT KNI_RETURNTYPE_INT
KNIDECL(com_sun_mmedia_DirectRecord_nStart) {
    jint handle = KNI_GetParameterAsInt(1);
    KNIPlayerInfo* pKniInfo = (KNIPlayerInfo*)handle;
    jint returnValue = 0;
    javacall_result ret;

    if (pKniInfo && pKniInfo->pNativeHandle) {
        JAVACALL_MM_ASYNC_EXEC(
            ret,
            javacall_media_start_recording(pKniInfo->pNativeHandle),
            pKniInfo->pNativeHandle, pKniInfo->appId, pKniInfo->playerId, JAVACALL_EVENT_MEDIA_RECORD_STARTED,
            returns_no_data
        );
        if (JAVACALL_OK == ret) {
            pKniInfo->recordState = RECORD_START;
            returnValue = 1;
        } else if (JAVACALL_WOULD_BLOCK != ret) {
            REPORT_ERROR1(LC_MMAPI, "[kni_record] Start recording fail handle=%d\n",
                pKniInfo->pNativeHandle);
        }
    }

    KNI_ReturnInt(returnValue);
}

/*  private native int nStop ( int handle ) ; */
KNIEXPORT KNI_RETURNTYPE_INT
KNIDECL(com_sun_mmedia_DirectRecord_nStop) {
    jint handle = KNI_GetParameterAsInt(1);
    KNIPlayerInfo* pKniInfo = (KNIPlayerInfo*)handle;
    jint returnValue = 0;
    javacall_result ret;

    if (pKniInfo && pKniInfo->pNativeHandle) {
        JAVACALL_MM_ASYNC_EXEC(
            ret,
            javacall_media_stop_recording(pKniInfo->pNativeHandle),
            pKniInfo->pNativeHandle, pKniInfo->appId, pKniInfo->playerId, JAVACALL_EVENT_MEDIA_RECORD_PAUSED,
            returns_no_data
        );
        if (JAVACALL_OK == ret) {
            pKniInfo->recordState = RECORD_STOP;
            returnValue = 1;
        } else if (JAVACALL_WOULD_BLOCK != ret) {
            REPORT_ERROR1(LC_MMAPI, "[kni_record] Stop recording fail handle=%d\n",
                pKniInfo->pNativeHandle);
        }
    }

    MMP_DEBUG_STR2("[kni_record] -nStop %d %d\n", pKniInfo->pNativeHandle, returnValue);

    KNI_ReturnInt(returnValue);
}

/*  private native int nPause ( int handle ) ; */
KNIEXPORT KNI_RETURNTYPE_INT
KNIDECL(com_sun_mmedia_DirectRecord_nPause) {
    jint handle = KNI_GetParameterAsInt(1);
    KNIPlayerInfo* pKniInfo = (KNIPlayerInfo*)handle;
    jint returnValue = 0;
    javacall_result ret;

    if (pKniInfo && pKniInfo->pNativeHandle) {
        JAVACALL_MM_ASYNC_EXEC(
            ret,
            javacall_media_pause_recording(pKniInfo->pNativeHandle),
            pKniInfo->pNativeHandle, pKniInfo->appId, pKniInfo->playerId, JAVACALL_EVENT_MEDIA_RECORD_PAUSED,
            returns_no_data
        );
        if (JAVACALL_OK == ret) {
            pKniInfo->recordState = RECORD_PAUSE;
            returnValue = 1;
        } else if (JAVACALL_WOULD_BLOCK != ret) {
            REPORT_ERROR1(LC_MMAPI, "[kni_record] Pause recording fail handle=%d\n",
                pKniInfo->pNativeHandle);
        }
    }

    MMP_DEBUG_STR2("[kni_record] -nPause handle=%d ret=%d\n", pKniInfo->pNativeHandle, returnValue);

    KNI_ReturnInt(returnValue);
}

/*  private native int nReset ( int handle ) ; */
KNIEXPORT KNI_RETURNTYPE_INT
KNIDECL(com_sun_mmedia_DirectRecord_nReset) {
    jint handle = KNI_GetParameterAsInt(1);
    KNIPlayerInfo* pKniInfo = (KNIPlayerInfo*)handle;
    jint returnValue = 0;
    javacall_result ret;

    if (pKniInfo && pKniInfo->pNativeHandle) {
        JAVACALL_MM_ASYNC_EXEC(
            ret,
            javacall_media_reset_recording(pKniInfo->pNativeHandle),
            pKniInfo->pNativeHandle, pKniInfo->appId, pKniInfo->playerId, JAVACALL_EVENT_MEDIA_RECORD_RESET,
            returns_no_data
        );
        if (JAVACALL_OK == ret) {
            pKniInfo->recordState = RECORD_RESET;
            returnValue = 1;
        } else if (JAVACALL_WOULD_BLOCK != ret) {
            REPORT_ERROR1(LC_MMAPI, "[kni_record] Reset recording fail handle=%d\n",
                pKniInfo->pNativeHandle);
        }
    }

    MMP_DEBUG_STR2("[kni_record] -nReset handle=%d ret=%d\n", pKniInfo->pNativeHandle, returnValue);

    KNI_ReturnInt(returnValue);
}

/*  private native int nCommit ( int handle ) ; */
KNIEXPORT KNI_RETURNTYPE_INT
KNIDECL(com_sun_mmedia_DirectRecord_nCommit) {
    jint handle = KNI_GetParameterAsInt(1);
    KNIPlayerInfo* pKniInfo = (KNIPlayerInfo*)handle;
    jint returnValue = 0;
    javacall_result ret;

    if (pKniInfo && pKniInfo->pNativeHandle) {
        JAVACALL_MM_ASYNC_EXEC(
            ret,
            javacall_media_commit_recording(pKniInfo->pNativeHandle),
            pKniInfo->pNativeHandle, pKniInfo->appId, pKniInfo->playerId, JAVACALL_EVENT_MEDIA_RECORD_COMMITTED,
            returns_no_data
        );
        if (JAVACALL_OK == ret) {
            pKniInfo->recordState = RECORD_COMMIT;
            returnValue = 1;
        } else if (JAVACALL_WOULD_BLOCK != ret) {
            REPORT_ERROR1(LC_MMAPI, "[kni_record] Commit recording fail handle=%d\n",
                pKniInfo->pNativeHandle);
        }
        returnValue = (JAVACALL_OK == ret ? 1 : 0);
    }

    MMP_DEBUG_STR2("[kni_record] -nCommit handle=%d ret=%d\n", pKniInfo->pNativeHandle, returnValue);
    
    KNI_ReturnInt(returnValue);
}

/*  private native int nClose ( int handle ) ; */
KNIEXPORT KNI_RETURNTYPE_INT
KNIDECL(com_sun_mmedia_DirectRecord_nClose) {
    jint handle = KNI_GetParameterAsInt(1);
    KNIPlayerInfo* pKniInfo = (KNIPlayerInfo*)handle;
    jint returnValue = 0;
    javacall_result ret;

    if (pKniInfo && pKniInfo->pNativeHandle) {
        ret = javacall_media_close_recording(pKniInfo->pNativeHandle);
        if (JAVACALL_OK == ret) {
            pKniInfo->recordState = RECORD_CLOSE;
        } else {
            REPORT_ERROR1(LC_MMAPI, "[kni_record] Close recording fail handle=%d\n",
                pKniInfo->pNativeHandle);
        }
        returnValue = (JAVACALL_OK == ret ? 1 : 0);
    }

    MMP_DEBUG_STR2("[kni_record] -nClose handle=%d ret=%d\n", pKniInfo->pNativeHandle, returnValue);

    KNI_ReturnInt(returnValue);
}

/*  private native int nSetSizeLimit ( int handle , int size ) ; */
KNIEXPORT KNI_RETURNTYPE_INT
KNIDECL(com_sun_mmedia_DirectRecord_nSetSizeLimit) {
    jint handle = KNI_GetParameterAsInt(1);
    long size = KNI_GetParameterAsInt(2);
    KNIPlayerInfo* pKniInfo = (KNIPlayerInfo*)handle;
    javacall_result ret;

    if (pKniInfo && pKniInfo->pNativeHandle) {
        ret = javacall_media_set_recordsize_limit(pKniInfo->pNativeHandle, (long*)&size);
        if (JAVACALL_FAIL == ret) {
            size = 0;
            REPORT_ERROR1(LC_MMAPI, "[kni_record] Set record size limit fail handle=%d\n", 
                pKniInfo->pNativeHandle);
        }
        if (JAVACALL_NOT_IMPLEMENTED == ret) {
            size = 0;
            REPORT_ERROR1(LC_MMAPI, "[kni_record] Set record size unsupported handle=%d\n", 
                pKniInfo->pNativeHandle);
        }
    }
    
    MMP_DEBUG_STR2("[kni_record] -nSetSizeLimit %d %d\n", pKniInfo->pNativeHandle, size);
    
    KNI_ReturnInt(size);
}

/*  private native int nGetRecordedSize ( int handle ) ; */
KNIEXPORT KNI_RETURNTYPE_INT
KNIDECL(com_sun_mmedia_DirectRecord_nGetRecordedSize) {

    jint handle = KNI_GetParameterAsInt(1);
    KNIPlayerInfo* pKniInfo = (KNIPlayerInfo*)handle;
    javacall_result ret;
    long size = 0;

    if (pKniInfo && pKniInfo->pNativeHandle) {
        ret = javacall_media_get_recorded_data_size(pKniInfo->pNativeHandle, &size);
        if (JAVACALL_FAIL == ret) {
            size = 0;
            REPORT_ERROR1(LC_MMAPI, "[kni_record] Get record size fail handle=%d\n", 
                pKniInfo->pNativeHandle);
        }
    }

    KNI_ReturnInt((jint)size);
}

/*  private native int nGetRecordedData ( int handle , int offset , int size , byte [ ] buffer ) ; */
KNIEXPORT KNI_RETURNTYPE_INT
KNIDECL(com_sun_mmedia_DirectRecord_nGetRecordedData) {
    jint handle = KNI_GetParameterAsInt(1);
    jint offset = KNI_GetParameterAsInt(2);
    jint size = KNI_GetParameterAsInt(3);
    KNIPlayerInfo* pKniInfo = (KNIPlayerInfo*)handle;
    jbyte* dataBuffer = NULL;
    jint returnValue = 0;
    javacall_result ret;

    KNI_StartHandles(2);
    KNI_DeclareHandle(bufferHandle);
    KNI_GetParameterAsObject(4, bufferHandle);

    dataBuffer = MMP_MALLOC(size);
    
    if (dataBuffer && pKniInfo && pKniInfo->pNativeHandle) {
        ret = javacall_media_get_recorded_data(pKniInfo->pNativeHandle, (char*)dataBuffer, offset, size);
        if (JAVACALL_OK == ret) {
            KNI_SetRawArrayRegion(bufferHandle, 0, size, (jbyte*)dataBuffer);
            returnValue = 1;
        } else {
            REPORT_ERROR1(LC_MMAPI, "[kni_record] Get record data fail handle=%d\n", 
                pKniInfo->pNativeHandle);
        }
        MMP_FREE(dataBuffer);
    } else {
        REPORT_ERROR(LC_MMAPI, "[kni_record] nGetRecordedData : buffer allocation failed\n");                
        KNI_ThrowNew(jsropOutOfMemoryError, NULL);
    }

    KNI_EndHandles();
    KNI_ReturnInt(returnValue);
}

/** private native String nGetRecordedType () ; */
KNIEXPORT KNI_RETURNTYPE_OBJECT
KNIDECL(com_sun_mmedia_DirectRecord_nGetRecordedType)
{
    jint handle = KNI_GetParameterAsInt(1);
    KNIPlayerInfo* pKniInfo = (KNIPlayerInfo*)handle;

    KNI_StartHandles(1);
    KNI_DeclareHandle(contentType);

    if (pKniInfo && pKniInfo->pNativeHandle) {
        int length;
        javacall_result ret;
        ret = javacall_media_get_record_content_type_length(pKniInfo->pNativeHandle, &length);
        if (ret == JAVACALL_OK && length > 0) {
            jchar* pString = MMP_MALLOC((length + 1) * sizeof(jchar));
            if (pString != NULL) {
                length += 1;
                ret = javacall_media_get_record_content_type(pKniInfo->pNativeHandle, pString, &length);
                if (ret == JAVACALL_OK) {
                    KNI_NewString(pString, length, contentType);
                } else {
                    KNI_ReleaseHandle(contentType);
                    REPORT_ERROR1(LC_MMAPI, "[kni_record] Get record content type fail handle=%d\n",
                        pKniInfo->pNativeHandle);          
                }
                MMP_FREE(pString);
            } else {
                REPORT_ERROR(LC_MMAPI, "[kni_record] Record content type buffer allocation fail\n");                
                KNI_ThrowNew(jsropOutOfMemoryError, NULL);
            }
        } else {
            KNI_ReleaseHandle(contentType);
            REPORT_ERROR(LC_MMAPI, "[kni_record] Record content type length is 0\n");
        }
    }

    KNI_EndHandlesAndReturnObject(contentType);
}

/*  private native void finalize () ; */
KNIEXPORT KNI_RETURNTYPE_VOID
KNIDECL(com_sun_mmedia_DirectRecord_finalize) 
{
    jint handle = (jint)NULL;
    KNIPlayerInfo* pKniInfo;

    KNI_StartHandles(4);
    KNI_DeclareHandle(instance);
    KNI_DeclareHandle(clazz);
    KNI_DeclareHandle(player_instance);
    KNI_DeclareHandle(player_clazz);
    
    /* Get this object instance and clazz */
    KNI_GetThisPointer(instance);
    KNI_GetObjectClass(instance, clazz);
    KNI_GetObjectField(instance, 
                       KNI_GetFieldID(clazz, "player", "Lcom/sun/mmedia/DirectPlayer;"), 
                       player_instance);
    if (KNI_FALSE == KNI_IsNullHandle(player_instance)) {
        KNI_GetObjectClass(player_instance, player_clazz);
        handle = KNI_GetIntField(player_instance, KNI_GetFieldID(player_clazz, "hNative", "I"));
    }
    
    if ((jint)NULL != handle) {
        pKniInfo = (KNIPlayerInfo*)handle;
        if (pKniInfo && pKniInfo->pNativeHandle) {
            MMP_DEBUG_STR1("[kni_record] stop recording by finalizer %d\n", pKniInfo->recordState);
            switch(pKniInfo->recordState) {
            case RECORD_START:
                javacall_media_stop_recording(pKniInfo->pNativeHandle);
                /* NOTE - Intentionally omit break */
            case RECORD_STOP:
                javacall_media_reset_recording(pKniInfo->pNativeHandle);
                /* NOTE - Intentionally omit break */
            case RECORD_RESET:
            case RECORD_COMMIT:
            case RECORD_INIT:
                javacall_media_close_recording(pKniInfo->pNativeHandle);
                break;
            }
            pKniInfo->recordState = RECORD_CLOSE;
        }
        KNI_SetBooleanField(instance, KNI_GetFieldID(clazz, "recording", "Z"), KNI_FALSE);
    }

    KNI_EndHandles();
    KNI_ReturnVoid();
}
