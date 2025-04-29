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
import java.util.Map;
import java.util.Set;

import net.onesaf.core.services.sim.interopmgt.hla.common.framework.message.ret.AbstractReturnMessage;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.message.rti.AbstractRTIMessage;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.rtihandle.InteractionClassRTIHandle;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.rtihandle.ParameterRTIHandle;
import net.onesaf.core.services.sim.interopmgt.hla.common.handler.RTIMessageHandler;
import net.onesaf.core.services.sim.interopmgt.hla.common.handler.ReturnMessageHandler;
import net.onesaf.core.services.sim.interopmgt.hla.common.handler.standard.AbstractInteractionHandler;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.ret.PublishInteractionClassReturn;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.ret.SendInteractionReturn;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.ret.SubscribeInteractionClassReturn;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.ret.UnpublishInteractionClassReturn;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.ret.UnsubscribeInteractionClassReturn;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.incoming.ReceiveInteraction;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.outgoing.PublishInteractionClass;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.outgoing.SendInteraction;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.outgoing.SubscribeInteractionClass;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.outgoing.UnpublishInteractionClass;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.outgoing.UnsubscribeInteractionClass;
import net.onesaf.core.services.sys.logger2.OneSAFLogger;

/**
 * This class is responsible for handling the HLA 1.3 RTI-dependent OOS and RTI
 * messages associated with the OOS/ODM and RTI/FOM Interactions. This mainly
 * provides support for calling converters subscribed to certain interactions
 * (OOS and RTI). It also provides call-back services for these converters that
 * isolate/hide/conceal other parts of the system.<br>
 * <br>
 * This handler is intended to be re-used as much as possible by external
 * developers. However, if it is not sufficient for a federate's needs, it can
 * be extended or replaced entirely by another handler.
 *
 * @author $Author$
 * @version $Revision$
 */
public class InteractionHandler extends AbstractInteractionHandler implements
    RTIMessageHandler, ReturnMessageHandler {

    /** Reference to the OneSAFLogger object. */
    private static OneSAFLogger logger =
        (OneSAFLogger) OneSAFLogger.getLogger(InteractionHandler.class);

    /**
     * Flag to indicate whether DEBUG logging has been enabled. This is mainly
     * used for preventing unnecessary string concatenation upon logging.
     */
    private static final boolean debugEnabled = logger.isDebugEnabled();

    // ====================== Singleton variable & method ======================

    /** Singleton reference. */
    protected static InteractionHandler instance = null;

    /** Singleton constructor. */
    protected InteractionHandler() {

    }

    /**
     *  Singleton method.
     *
     * @return the singleton reference
     */
    public static InteractionHandler getInstance() {
        if (instance == null) {
            instance = new InteractionHandler();
        }
        return instance;
    }

    // ======================== Process Message Methods ========================

    /**
     * Method to handle a newly occurring RTI message. Calls appropriate methods
     * on converters for each RTI message.
     * 
     * @param rtiMessage <code>AbstractRTIMessage</code> to be processed.
     */
    @Override
    public void handleMessage(AbstractRTIMessage rtiMessage) {

        // Depending on the type of the message, call different methods of the
        // converter (only one message type needed)
        if (rtiMessage instanceof ReceiveInteraction) {

            // Find corresponding converter for the object specified by the
            // rtiMessage
            //
            ReceiveInteraction msg = (ReceiveInteraction) rtiMessage;

            receiveInteraction(msg.getInteractionClassHandle(), msg
                .getReceivedInteraction(), msg.getUserSuppliedTag());
        }
    }

    /**
     * Method to handle a newly occurring Return message. Logs any exception.
     * 
     * @param returnMessage <code>AbstractReturnMessage</code> to be processed.
     */
    @Override
    public void handleMessage(AbstractReturnMessage returnMessage) {

        // Check for & log exception
        if (returnMessage.getException() != null) {
            logger.error("Exception in return message: ", returnMessage
                .getException());
        }
    }

    /**
     * Register rti msg interest.
     */
    @Override
    protected void registerRTIMsgInterest() {
        transManager.registerRTIInterest(ReceiveInteraction.class, this);
    }

    /**
     * Register return message interest.
     */
    @Override
    protected void registerReturnMessageInterest() {
        Set<Class <? extends AbstractReturnMessage>> returnMsgInterests = new HashSet<>();
        returnMsgInterests.add(PublishInteractionClassReturn.class);
        returnMsgInterests.add(SubscribeInteractionClassReturn.class);
        returnMsgInterests.add(UnpublishInteractionClassReturn.class);
        returnMsgInterests.add(UnsubscribeInteractionClassReturn.class);
        returnMsgInterests.add(SendInteractionReturn.class);
        transManager.registerReturnInterest(this, returnMsgInterests);
    }

    /**
     * Send interaction to rti.
     *
     * @param classHandle the class handle
     * @param outputData the output data
     * @param outgoingTag the outgoing tag
     */
    @Override
    public void sendInteractionToRTI(InteractionClassRTIHandle classHandle,
        Map<ParameterRTIHandle, byte[]> outputData, String outgoingTag) {

        SendInteraction sendIntMsg = new SendInteraction();

        sendIntMsg.setInteractionHandle(classHandle);
        sendIntMsg.setTag(outgoingTag.getBytes());
        sendIntMsg.setAttrValueSet(outputData);

        sendIntMsg.send();
    }

    /**
     * Send publish interaction class message.
     *
     * @param interactionClassHandle the interaction class handle
     */
    @Override
    public void sendPublishInteractionClassMessage(
        InteractionClassRTIHandle interactionClassHandle) {

        PublishInteractionClass pubIntrClassMsg = new PublishInteractionClass();

        pubIntrClassMsg.setInteractionClassHandle(interactionClassHandle);
        pubIntrClassMsg.send();
    }

    /**
     * Send subscribe interaction class message.
     *
     * @param interactionClassHandle the interaction class handle
     */
    @Override
    public void sendSubscribeInteractionClassMessage(
        InteractionClassRTIHandle interactionClassHandle) {

        SubscribeInteractionClass subIntrClassMsg =
            new SubscribeInteractionClass();

        subIntrClassMsg.setInteractionClassHandle(interactionClassHandle);
        subIntrClassMsg.send();
    }

    /**
     * Send unpublish interaction class message.
     *
     * @param interactionClassHandle the interaction class handle
     */
    @Override
    public void sendUnpublishInteractionClassMessage(
        InteractionClassRTIHandle interactionClassHandle) {

        UnpublishInteractionClass unPubIntrClassMsg =
            new UnpublishInteractionClass();

        unPubIntrClassMsg.setInteractionClassHandle(interactionClassHandle);
        unPubIntrClassMsg.send();
    }

    /**
     * Send unsubscribe interaction class message.
     *
     * @param interactionClassHandle the interaction class handle
     */
    @Override
    public void sendUnsubscribeInteractionClassMessage(
        InteractionClassRTIHandle interactionClassHandle) {

        UnsubscribeInteractionClass unSubIntrClassMsg =
            new UnsubscribeInteractionClass();

        unSubIntrClassMsg.setInteractionClassHandle(interactionClassHandle);
        unSubIntrClassMsg.send();
    }
}
