/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#include "precompiled_sd.hxx"

#include "SlsQueueProcessor.hxx"
#include "SlsCacheConfiguration.hxx"
#include "SlsRequestQueue.hxx"

namespace sd { namespace slidesorter { namespace cache {


//=====  QueueProcessor  ======================================================

QueueProcessor::QueueProcessor (
    RequestQueue& rQueue,
    const ::boost::shared_ptr<BitmapCache>& rpCache,
    const Size& rPreviewSize,
    const bool bDoSuperSampling,
    const SharedCacheContext& rpCacheContext)
    : maMutex(),
      maTimer(),
      mnTimeBetweenHighPriorityRequests (10/*ms*/),
      mnTimeBetweenLowPriorityRequests (100/*ms*/),
      mnTimeBetweenRequestsWhenNotIdle (1000/*ms*/),
      maPreviewSize(rPreviewSize),
      mbDoSuperSampling(bDoSuperSampling),
      mpCacheContext(rpCacheContext),
      mrQueue(rQueue),
      mpCache(rpCache),
      maBitmapFactory(),
      mbIsPaused(false)
{
    // Look into the configuration if there for overriding values.
    ::com::sun::star::uno::Any aTimeBetweenReqeusts;
    aTimeBetweenReqeusts = CacheConfiguration::Instance()->GetValue(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("TimeBetweenHighPriorityRequests")));
    if (aTimeBetweenReqeusts.has<sal_Int32>())
        aTimeBetweenReqeusts >>= mnTimeBetweenHighPriorityRequests;
    
    aTimeBetweenReqeusts = CacheConfiguration::Instance()->GetValue(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("TimeBetweenLowPriorityRequests")));
    if (aTimeBetweenReqeusts.has<sal_Int32>())
        aTimeBetweenReqeusts >>= mnTimeBetweenLowPriorityRequests;

    aTimeBetweenReqeusts = CacheConfiguration::Instance()->GetValue(
        ::rtl::OUString(RTL_CONSTASCII_USTRINGPARAM("TimeBetweenRequestsDuringShow")));
    if (aTimeBetweenReqeusts.has<sal_Int32>())
        aTimeBetweenReqeusts >>= mnTimeBetweenRequestsWhenNotIdle;

    maTimer.SetTimeoutHdl (LINK(this,QueueProcessor,ProcessRequestHdl));
    maTimer.SetTimeout (mnTimeBetweenHighPriorityRequests);
}





QueueProcessor::~QueueProcessor (void)
{
}




void QueueProcessor::Start (int nPriorityClass)
{
    if (mbIsPaused)
        return;
    if ( ! maTimer.IsActive())
    {
        if (nPriorityClass == 0)
            maTimer.SetTimeout (mnTimeBetweenHighPriorityRequests);
        else
            maTimer.SetTimeout (mnTimeBetweenLowPriorityRequests);
        maTimer.Start();
    }
}




void QueueProcessor::Stop (void)
{
    if (maTimer.IsActive())
        maTimer.Stop();
}




void QueueProcessor::Pause (void)
{
    mbIsPaused = true;
}




void QueueProcessor::Resume (void)
{
    mbIsPaused = false;
    if ( ! mrQueue.IsEmpty())
        Start(mrQueue.GetFrontPriorityClass());
}




void QueueProcessor::Terminate (void)
{
}




void QueueProcessor::SetPreviewSize (
    const Size& rPreviewSize,
    const bool bDoSuperSampling)
{
    maPreviewSize = rPreviewSize;
    mbDoSuperSampling = bDoSuperSampling;
}




IMPL_LINK(QueueProcessor, ProcessRequestHdl, Timer*, EMPTYARG)
{
    ProcessRequests();
    return 1;
}




void QueueProcessor::ProcessRequests (void)
{
    OSL_ASSERT(mpCacheContext.get()!=NULL);

    // Never process more than one request at a time in order to prevent the
    // lock up of the edit view.
    if ( ! mrQueue.IsEmpty()
        && ! mbIsPaused
        &&  mpCacheContext->IsIdle())
    {
        CacheKey aKey = NULL;
        RequestPriorityClass ePriorityClass (NOT_VISIBLE);
        {
            ::osl::MutexGuard aGuard (mrQueue.GetMutex());

            if ( ! mrQueue.IsEmpty())
            {
                // Get the request with the highest priority from the queue.
                ePriorityClass = mrQueue.GetFrontPriorityClass();
                aKey = mrQueue.GetFront();
                mrQueue.PopFront();
            }
        }

        if (aKey != NULL)
            ProcessOneRequest(aKey, ePriorityClass);
    }

    // Schedule the processing of the next element(s).
    {
        ::osl::MutexGuard aGuard (mrQueue.GetMutex());
        if ( ! mrQueue.IsEmpty())
            Start(mrQueue.GetFrontPriorityClass());
    }
}




void QueueProcessor::ProcessOneRequest (
    CacheKey aKey,
    const RequestPriorityClass ePriorityClass)
{
    try
    {
        ::osl::MutexGuard aGuard (maMutex);

        // Create a new preview bitmap and store it in the cache.
        if (mpCache.get() != NULL
            && mpCacheContext.get() != NULL)
        {
            const SdPage* pSdPage = dynamic_cast<const SdPage*>(mpCacheContext->GetPage(aKey));
            if (pSdPage != NULL)
            {
                const Bitmap aPreview (
                    maBitmapFactory.CreateBitmap(*pSdPage, maPreviewSize, mbDoSuperSampling));
                mpCache->SetBitmap (pSdPage, aPreview, ePriorityClass!=NOT_VISIBLE);

                // Initiate a repaint of the new preview.
                mpCacheContext->NotifyPreviewCreation(aKey, aPreview);
            }
        }
    }
    catch (::com::sun::star::uno::RuntimeException aException)
    {
        (void) aException;
        OSL_ASSERT("RuntimeException caught in QueueProcessor");
    }
    catch (::com::sun::star::uno::Exception aException)
    {
        (void) aException;
        OSL_ASSERT("Exception caught in QueueProcessor");
    }
}




void QueueProcessor::RemoveRequest (CacheKey aKey)
{
    (void)aKey;
    // See the method declaration above for an explanation why this makes sense.
    ::osl::MutexGuard aGuard (maMutex);
}




void QueueProcessor::SetBitmapCache (
    const ::boost::shared_ptr<BitmapCache>& rpCache)
{
    mpCache = rpCache;
}


} } } // end of namespace ::sd::slidesorter::cache
