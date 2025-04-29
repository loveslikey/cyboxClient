/*
 * Classification:  Unclassified
 *
 * Project Name:  OneSAF
 *
 * DISTRIBUTION STATEMENT C.
 *
 * Distribution is authorized to U.S. Government Agencies and their
 * contractors. Export Controlled: 23 August, 2012. Other requests for
 * this document shall be referred to U.S. Army PEO STRI.
 *
 */
package net.onesaf.core.services.sim.interopmgt.hla.hla13.handler.standard;

import java.util.HashSet;
import java.util.Set;

import net.onesaf.core.services.sim.interopmgt.hla.common.framework.message.ret.AbstractReturnMessage;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.message.rti.AbstractRTIMessage;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.rtihandle.AttributeRTIHandle;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.rtihandle.ObjectClassRTIHandle;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.rtihandle.ObjectRTIHandle;
import net.onesaf.core.services.sim.interopmgt.hla.common.handler.RTIMessageHandler;
import net.onesaf.core.services.sim.interopmgt.hla.common.handler.ReturnMessageHandler;
import net.onesaf.core.services.sim.interopmgt.hla.common.handler.standard.AbstractObjectHandler;
import net.onesaf.core.services.sim.interopmgt.hla.common.service.HeartbeatService;
import net.onesaf.core.services.sim.interopmgt.hla.common.service.TimeoutService;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.ret.PublishObjectClassReturn;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.ret.RegisterObjectInstanceReturn;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.ret.RequestObjAttrValUpdateReturn;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.ret.SubscribeObjectClassAttributesReturn;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.ret.UnpublishObjectClassReturn;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.ret.UnsubscribeObjectClassReturn;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.ret.UpdateAttributeValuesReturn;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.incoming.DiscoverObjectInstance;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.incoming.ProvideAttrValUpdate;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.incoming.ReflectAttributeValues;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.incoming.RemoveObjectInstance;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.outgoing.PublishObjectClass;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.outgoing.RegisterObjectInstance;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.outgoing.RequestObjAttrValUpdate;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.outgoing.SubscribeObjectClassAttributes;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.outgoing.UnpublishObjectClass;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.outgoing.UnsubscribeObjectClass;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.service.HLA13HeartbeatService;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.service.HLA13TimeoutService;
import net.onesaf.core.services.sys.logger2.OneSAFLogger;

/**
 * This class is responsible for handling the HLA 1.3 RTI-dependent OOS and RTI
 * messages associated with the OOS/ODM and RTI/FOM Objects.  This mainly
 * provides support for calling converters subscribed to certain objects (OOS
 * and RTI).  It also provides call-back services for these converters that
 * isolate/hide/conceal other parts of the system.<br>
 * <br>
 * This handler is intended to be re-used as much as possible by external
 * developers.  However, if it is not sufficient for a federate's needs, it can
 * be extended or replaced entirely by another handler.<br>
 * <br>
 * This class also provides object tracking of local and remote objects.  This
 * tracking is handled in some of the call-back methods to be used by
 * converters.<br>
 * <br>
 * This handler is also responsible ("interested") in Return messages,
 * specifically the RegisterObjectInstanceReturn message type.
 *
 * @author $Author$
 * @version $Revision$
 */
public class ObjectHandler extends AbstractObjectHandler
        implements RTIMessageHandler, ReturnMessageHandler {

    /** Reference to the OneSAFLogger object. */
    private static OneSAFLogger logger = (OneSAFLogger) OneSAFLogger.getLogger(
            ObjectHandler.class);

    /** Flag to indicate if the DEBUG level of logging is enabled. */
    private final boolean debugEnabled = logger.isDebugEnabled();

    // ====================== Singleton variable & method ======================

    /** Singleton reference. */
    protected static ObjectHandler instance = null;

    /** Singleton constructor. */
    protected ObjectHandler() {
    }

    /**
     *  Singleton method.
     *
     * @return the singleton reference
     */
    public static ObjectHandler getInstance() {
        if (instance == null) {
            instance = new ObjectHandler();
        }
        return instance;
    }

    // ========================= Handle Message Methods ========================

    /**
     * Method to handle a newly occurring RTI messages.  Calls appropriate
     * methods on converters for each RTI message.
     *
     * @param rtiMessage <code>AbstractRTIMessage</code> to be handled.
     */
    @Override
    public void handleMessage(AbstractRTIMessage rtiMessage) {

        // Depending on the type of the message, call different methods of the
        // converter
    	
    	
        if (rtiMessage instanceof DiscoverObjectInstance) {
        	
            DiscoverObjectInstance msg = (DiscoverObjectInstance) rtiMessage;

            discoverObjectInstance(msg.getObjectHandle(), msg.getObjectName());
        } else if (rtiMessage instanceof RemoveObjectInstance) {
        	
            RemoveObjectInstance msg = (RemoveObjectInstance) rtiMessage;

            ObjectRTIHandle objHandleRTI = msg.getObjectHandle();

            removeObjectInstance(objHandleRTI);
        } else if (rtiMessage instanceof ReflectAttributeValues) {
        	
            ReflectAttributeValues msg = (ReflectAttributeValues) rtiMessage;

            ObjectRTIHandle objHandleRTI = msg.getObjectHandle();
            if (objHandleRTI != null) {
            	reflectAttributeValues(objHandleRTI,
            			msg.getReflectedAttributes(), msg.getTheTag());
            } else {
            	logger.error("Object RTI Handle is null, cannot reflect attribute values");
            }
        } else if (rtiMessage instanceof ProvideAttrValUpdate) {
            ProvideAttrValUpdate msg = (ProvideAttrValUpdate) rtiMessage;

            provideAttributeValueUpdate(msg.getObjectHandle(),
                msg.getAttrs());
        }
    }

    /**
     * Method to handle a newly occurring Return message.<br>
     * <br>
     * Default handling of the REGISTER_OBJECT_INSTANCE_RESULT* RTI message.
     * Sets the proxy object on the RTI handle, adds to
     * <code>localTrackedObjects</code> list.  Hands control to associated
     * converter for the object to allow converter control (i.e. - force out
     * update if appropriate).<br>
     * <br>
     * * - This is a response to our RTI handle request.
     *
     * @param retMessage <code>AbstractReturnMessage</code> to be handled.
     */
    @Override
    public void handleMessage(AbstractReturnMessage retMessage) {

        // Check for error condition before handling msg & passing to converter
        if (retMessage.getException() != null) {
            logger.error(retMessage + " not being handled and passed to " +
                         "converter because Exception.",
                         retMessage.getException());
            return;
        }

        if (retMessage instanceof RegisterObjectInstanceReturn) {

            RegisterObjectInstanceReturn msg = (RegisterObjectInstanceReturn)
                retMessage;

            // Update local data before forwarding to converter
            ObjectRTIHandle objectHandle = msg.getObjectHandle();
            Object oosObject = msg.getOOSObject();

            registerObjectInstanceReturn(objectHandle, oosObject);
        }
    }

    @Override
    protected HeartbeatService createHeartbeatService(int objBuckets,
            float interval, int numThreads) {

        return new HLA13HeartbeatService(objBuckets, interval, numThreads);
    }

    /**
     * Creates the timeout service.
     *
     * @param objBuckets the obj buckets
     * @param interval the interval
     * @return the timeout service
     */
    @Override
        protected TimeoutService createTimeoutService(int objBuckets, float interval,
                                                      int timeoutChecksPerInterval) {
        return new HLA13TimeoutService(objBuckets, interval,
                                       timeoutChecksPerInterval);
    }

    /**
     * Register rti msg interest.
     */
    @Override
    protected void registerRTIMsgInterest() {
        Set<Class <? extends AbstractRTIMessage>> rtiMsgInterests = new HashSet<>();
        rtiMsgInterests.add(DiscoverObjectInstance.class);
        rtiMsgInterests.add(RemoveObjectInstance.class);
        rtiMsgInterests.add(ReflectAttributeValues.class);
        rtiMsgInterests.add(ProvideAttrValUpdate.class);
        transManager.registerRTIInterest(this, rtiMsgInterests);
    }

    /**
     * Register return message interest.
     */
    @Override
    protected void registerReturnMessageInterest() {
        Set<Class <? extends AbstractReturnMessage>> returnMsgInterests = new HashSet<>();
        returnMsgInterests.add(RegisterObjectInstanceReturn.class);
        returnMsgInterests.add(UpdateAttributeValuesReturn.class);
        returnMsgInterests.add(RequestObjAttrValUpdateReturn.class);
        returnMsgInterests.add(PublishObjectClassReturn.class);
        returnMsgInterests.add(SubscribeObjectClassAttributesReturn.class);
        returnMsgInterests.add(UnpublishObjectClassReturn.class);
        returnMsgInterests.add(UnsubscribeObjectClassReturn.class);
        transManager.registerReturnInterest(this, returnMsgInterests);
    }

    /**
     * Send register object instance message.
     *
     * @param classHandle the class handle
     * @param objTag the obj tag
     * @param proxy the proxy
     */
    @Override
    protected void sendRegisterObjectInstanceMessage(
            ObjectClassRTIHandle classHandle, String objTag, Object proxy) {

        RegisterObjectInstance regObjMsg = new RegisterObjectInstance();

        regObjMsg.setObjectClassHandle(classHandle);
        regObjMsg.setTag(objTag);
        regObjMsg.setOOSObject(proxy);

        regObjMsg.send();
    }

    /**
     * Send request obj attr val update message.
     *
     * @param objHandle the obj handle
     * @param msgAttrSet the msg attr set
     */
    @Override
    public void sendRequestObjAttrValUpdateMessage(ObjectRTIHandle objHandle,
            Set<AttributeRTIHandle> msgAttrSet) {

        RequestObjAttrValUpdate allAttrReqMsg = new RequestObjAttrValUpdate();

        allAttrReqMsg.setObjectHandle(objHandle);
        // copy list into message
        Set<AttributeRTIHandle> attrValueSet = allAttrReqMsg.getAttrValueSet();
        for (AttributeRTIHandle curHandle : msgAttrSet) {
            attrValueSet.add(curHandle);
        }

        allAttrReqMsg.send();
    }

    /**
     * Send publish object class message.
     *
     * @param objClassHandle the obj class handle
     * @param attrHandles the attr handles
     */
    @Override
    public void sendPublishObjectClassMessage(
            ObjectClassRTIHandle objClassHandle,
            Set<AttributeRTIHandle> attrHandles) {

        PublishObjectClass pubObjClassMsg = new PublishObjectClass();

        pubObjClassMsg.setObjectClassHandle(objClassHandle);
        pubObjClassMsg.setAttributeHandleSet(attrHandles);

        pubObjClassMsg.send();
    }

    /**
     * Send subscribe object class attributes message.
     *
     * @param objClassHandle the obj class handle
     * @param attrHandles the attr handles
     */
    @Override
    public void sendSubscribeObjectClassAttributesMessage(
            ObjectClassRTIHandle objClassHandle,
            Set<AttributeRTIHandle> attrHandles) {

        SubscribeObjectClassAttributes subObjClassMsg = new
            SubscribeObjectClassAttributes();

        subObjClassMsg.setObjectClassHandle(objClassHandle);
        subObjClassMsg.setAttributeHandleSet(attrHandles);

        subObjClassMsg.send();
    }

    /**
     * Send unpublish object class message.
     *
     * @param objClassHandle the obj class handle
     */
    @Override
    public void sendUnpublishObjectClassMessage(
            ObjectClassRTIHandle objClassHandle) {

        UnpublishObjectClass unPubObjClassMsg = new UnpublishObjectClass();

        unPubObjClassMsg.setObjectClassHandle(objClassHandle);

        unPubObjClassMsg.send();
    }

    /**
     * Send unsubscribe object class message.
     *
     * @param objClassHandle the obj class handle
     */
    @Override
    public void sendUnsubscribeObjectClassMessage(
            ObjectClassRTIHandle objClassHandle) {

        UnsubscribeObjectClass unSubObjClassMsg = new UnsubscribeObjectClass();

        unSubObjClassMsg.setObjectClassHandle(objClassHandle);

        unSubObjClassMsg.send();
    }
    
    @Override
    public void restoreObject(Object object, String fomName, String objectTag) {
    	// not implemented
    }
    

}
