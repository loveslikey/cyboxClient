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
/*
 * Copyrights 2006-2018
 *               Leidos
 *               Cole Engineering Services
 *               Cornerstone Software Solutions
 *               Dignitas Technologies LLC
 *               Productivity Apex
 *               SimIS
 *               All rights reserved.
 */
package net.onesaf.core.services.sim.interopmgt.hla.hla13.handler.standard;

import java.lang.reflect.Field;
import java.util.Collection;

import hla.rti.FederateAlreadyExecutionMember;
import hla.rti.FederationExecutionAlreadyExists;
import hla.rti.FederationExecutionDoesNotExist;
import hla.rti.ResignAction;

import net.onesaf.core.services.data.dm.mdm.mgmt.SimulationStateEnum;
import net.onesaf.core.services.data.dm.mdm.mgmt.intr.CheckpointRestoreFailedAlert;
import net.onesaf.core.services.data.dm.mdm.mgmt.intr.CheckpointSaveFailedAlert;
import net.onesaf.core.services.data.dminit.DataModelInitializer;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.amb.OneSAFFedAmbassador;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.message.oos.SimStateChanged;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.message.ret.AbstractReturnMessage;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.message.rti.AbstractRTIMessage;
import net.onesaf.core.services.sim.interopmgt.hla.common.handler.RTIMessageHandler;
import net.onesaf.core.services.sim.interopmgt.hla.common.handler.ReturnMessageHandler;
import net.onesaf.core.services.sim.interopmgt.hla.common.handler.standard.AbstractFedManagementHandler;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.amb.OneSAFHLA13RTIAmbassador;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.message.oos.AbstractOOSMessage;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.ret.CreateFederationExecReturn;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.ret.DestroyFederationExecReturn;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.ret.JoinFederationExecReturn;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.ret.ResignFederationExecReturn;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.incoming.FederationNotRestored;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.incoming.FederationNotSaved;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.incoming.FederationRestoreBegun;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.incoming.FederationRestored;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.incoming.FederationSaved;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.incoming.InitiateFederateRestore;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.incoming.InitiateFederateSave;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.outgoing.CreateFederationExecution;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.outgoing.DestroyFederationExecution;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.outgoing.JoinFederationExecution;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.outgoing.ResignFederationExecution;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.rtihandle.HLA13RTIHandleManager;
import net.onesaf.core.services.sim.simeng.SimulationEngine;
import net.onesaf.core.services.sim.simeng.checkpointmgt.CheckpointInformation;
import net.onesaf.core.services.sim.simeng.checkpointmgt.CheckpointManager;
import net.onesaf.core.services.sys.composition.ComponentManager;
import net.onesaf.core.services.sys.gui.utils.Utils;
import net.onesaf.core.services.sys.logger2.OneSAFLogger;
import net.onesaf.core.services.sys.odb.InteractionListener;
import net.onesaf.core.services.sys.odb.ObjectDatabase;
import net.onesaf.core.services.sim.simeng.common.DistributionStateManager;

/**
 * This class is responsible for HLA 1.3 RTI-dependent handling of creation,
 * joining, resigning, and destruction of the federate/federation.
 * 
 * @version $Revision$
 * @author $Author$
 */
public class FedManagementHandler extends AbstractFedManagementHandler
    implements RTIMessageHandler, ReturnMessageHandler, InteractionListener {

    /**
     * Reference to the OneSAFLogger object.
     */
    private static OneSAFLogger logger =
        (OneSAFLogger) OneSAFLogger.getLogger(FedManagementHandler.class);

    /**
     * Flag to indicate whether DEBUG logging has been enabled. This is mainly
     * used for preventing unnecessary string concatenation upon logging.
     */
    private static final boolean debugEnabled = logger.isDebugEnabled();

    /**
     * Reference to the ObjectDatabase object.
     */
    private static ObjectDatabase odb;

    /**
     * Reference to the singleton instance of this class.
     */
    protected static FedManagementHandler instance = null;

    /**
     * Reference to the OneSAFHLA13RTIAmbassador object.
     */
    protected OneSAFHLA13RTIAmbassador rtiAmbassador = null;

    /**
     * Boolean indicating if we are waiting on the Restored simulation state
     * after restoring a saved checkpoint based on an RTI request.
     */
    protected boolean waitingForRestored = false;

    /**
     * Protected constructor.
     */
    protected FedManagementHandler() {

        DataModelInitializer mdm = ComponentManager.getInstance(DataModelInitializer.class);
        odb = mdm.getObjectDatabase();

        odb.getInteractionManager().addListener(
            CheckpointSaveFailedAlert.class, this, null);
        odb.getInteractionManager().addListener(
            CheckpointRestoreFailedAlert.class, this, null);
    }

    /**
     * Returns a reference to the singleton instance of this class.
     * 
     * @return a reference to the singleton instance of this class
     */
    public static FedManagementHandler getInstance() {

        if (instance == null) {
            instance = new FedManagementHandler();
        }

        return instance;
    }

    @Override
    public void registerOOSMessageInterest() {

        transManager.registerOOSInterest(SimStateChanged.class, this);
    }

    @Override
    public void registerReturnMessageInterest() {

        transManager.registerReturnInterest(CreateFederationExecReturn.class,
            this);
        transManager.registerReturnInterest(JoinFederationExecReturn.class,
            this);
        transManager.registerReturnInterest(ResignFederationExecReturn.class,
            this);
        transManager.registerReturnInterest(DestroyFederationExecReturn.class,
            this);
    }

    @Override
    public void registerRTIMessageInterest() {

        transManager.registerRTIInterest(InitiateFederateSave.class, this);
        transManager.registerRTIInterest(FederationSaved.class, this);
        transManager.registerRTIInterest(FederationNotSaved.class, this);
        transManager.registerRTIInterest(FederationRestoreBegun.class, this);
        transManager.registerRTIInterest(InitiateFederateRestore.class, this);
        transManager.registerRTIInterest(FederationRestored.class, this);
        transManager.registerRTIInterest(FederationNotRestored.class, this);
    }

    @Override
    public void sendCreateFederationExecutionMessage(String fedName,
        String FEDFileURL) {

        CreateFederationExecution createFedExecMsg =
            new CreateFederationExecution();

        createFedExecMsg.setFederationName(fedName);
        createFedExecMsg.setFedFilePath(FEDFileURL);

        createFedExecMsg.send();
    }

    @Override
    public void sendDestroyFederationExecutionMessage(String federationName) {

        DestroyFederationExecution destroyFedExecMsg =
            new DestroyFederationExecution();

        destroyFedExecMsg.setFederationName(federationName);

        destroyFedExecMsg.send();
    }

    @Override
    public void sendJoinFederationExecutionMessage(String federationName,
        String federateName, OneSAFFedAmbassador fedAmb) {

        // Join federation
        JoinFederationExecution joinFedExecMsg = new JoinFederationExecution();

        joinFedExecMsg.setFederateName(federateName);
        joinFedExecMsg.setFederationName(federationName);
        // RTI ambassaor requires reference to the federate ambassador, so
        // provide it via the message.
        joinFedExecMsg.setFedAmbassador(fedAmb);

        joinFedExecMsg.send();
    }

    @Override
    public void sendResignFederationExecutionMessage(String resignAction) {

        ResignFederationExecution resignFedExecMsg =
            new ResignFederationExecution();

        resignFedExecMsg.setTheAction(getResignAction(resignAction));

        resignFedExecMsg.send();
    }

    public int getResignAction(String resignAction) {

        int retAction = ResignAction.DELETE_OBJECTS_AND_RELEASE_ATTRIBUTES;

        if (resignAction != null && resignAction.length() > 0) {
            // use introspection/reflection to see the provided String is a
            // valid resign action of the RTI
            try {
                Field action = ResignAction.class.getField(resignAction);
                retAction = action.getInt(ResignAction.class);
                if (logger.isDebugEnabled()) {
                    logger.debug("Resign action '" + resignAction + "' => '"
                        + retAction + "'");
                }
            } catch (Exception ex) {
                logger.error("Invalid RTI resign action: '" + resignAction
                    + "' - using default " + "'" + retAction + "'", ex);
            }
        }

        return retAction;
    }

    // *************************************************************************
    // *************************** Handler methods *****************************
    // *************************************************************************

    @Override
    public void handleMessage(AbstractOOSMessage oosMessage) {

        super.handleMessage(oosMessage);
        
        if (oosMessage instanceof SimStateChanged) {

            SimulationStateEnum simulationState =
                ((SimStateChanged) oosMessage).getSimState();

            if(logger.isDebugEnabled()) {
	            logger.debug("FedManagementHandler received sim state: "
	                + simulationState);
            }

            if (simulationState.equals(SimulationStateEnum.RESTORED)
                && waitingForRestored) {

            	if(logger.isDebugEnabled()) {
            		logger.debug("Calling federateRestoreComplete()");
            	}

                // inform the RTI that this federate has completed restoring its
                // federate-managed state
                try {

                    rtiAmbassador.federateRestoreComplete();
                } catch (Exception e) {
                    // notification of RTI -- we don't care if there is an error
                }

                waitingForRestored = false;
            }
        }
    }

    @Override
    public void handleMessage(AbstractRTIMessage incomingMessage) {

        if (incomingMessage instanceof InitiateFederateSave) {

            logger.info("FedManagementHandler - handling InitiateFederateSave");

            SimulationStateEnum simulationState;
            DistributionStateManager distributionStateManager;

            SimulationEngine simulationEngine =
                (SimulationEngine) ComponentManager
                    .getInstance(SimulationEngine.class.getName());
            distributionStateManager =
                simulationEngine.getDistributionStateManager();
            simulationState = distributionStateManager.getSimulationState();

            if (simulationState != SimulationStateEnum.RUNNING) {

                // if OneSAF is not in the running state we cannot perform a
                // checkpoint save. Call back the RTI with
                // federateSaveNotComplete().
                try {

                    if (rtiAmbassador == null) {

                        rtiAmbassador =
                            (OneSAFHLA13RTIAmbassador) hlaManager
                                .getOneSAFRTIAmbassador();
                    }

                    if(logger.isDebugEnabled()) {
	                    logger.debug("OneSAF cannot perform checkpoint save "
	                        + "because it is not in the RUNNING state");
                    }

                    rtiAmbassador.federateSaveNotComplete();
                } catch (Exception e) {
                    logger.error("Caught exception: " + e);
                }
            } else {

                try {

                    if (rtiAmbassador == null) {

                        rtiAmbassador =
                            (OneSAFHLA13RTIAmbassador) hlaManager
                                .getOneSAFRTIAmbassador();
                    }

                    // pause processing of messages by the TranslationManager
                    transManager.pauseMessageProcessing();

                    // inform the RTI that this federate is saving it's
                    // federate-
                    // managed state
                    rtiAmbassador.federateSaveBegun();
                    
                    if(logger.isDebugEnabled()) {
                    	logger.debug("federateSaveBegun");
                    }
                    
                    // Save the checkpoint to the Master Node.
                    // Currently used to manually restore via the MCT.
                    String nodes[] = {distributionStateManager.getMasterNode()};

                    System.out.println("CheckPoint " + ((InitiateFederateSave) incomingMessage).getLabel() +
                            " with sim time = " + Utils.convertTimeMillisWithDays(oosAmbassador.getSimulationTime()));
                    CheckpointManager.getInstance().scheduleCheckpointRequest(
                            ((InitiateFederateSave) incomingMessage).getLabel(),
                            oosAmbassador.getSimulationTime(), 
                            0,
                            false,
                            nodes);

                    // inform the RTI that this federate has completed saving
                    // its
                    // federate-managed state
                    rtiAmbassador.federateSaveComplete();
                    
                    if(logger.isDebugEnabled()) {
                    	logger.debug("federateSaveComplete");
                    }

                } catch (Exception e) {

                    logger.error(
                        "FedManagementHandler - caught Exception while"
                            + " saving checkpoint", e);

                    try {

                        // inform the RTI that this federate has not completed
                        // saving its federate-managed state
                        rtiAmbassador.federateSaveNotComplete();
                    } catch (Exception e2) {
                        // notification of RTI -- we don't care if there is an
                        // error
                    }
                } finally {

                    // resume processing of messages by the TranslationManager
                    transManager.resumeMessageProcessing();
                }
            }
        } else if (incomingMessage instanceof FederationSaved) {
            System.out.println("FedManagementHandler - handling FederationSaved");
            logger.info("FedManagementHandler - handling FederationSaved");

        } else if (incomingMessage instanceof FederationNotSaved) {
            System.out.println("FedManagementHandler - handling Federation NOT Saved");
            logger.info("FedManagementHandler - handling FederationNotSaved");

        } else if (incomingMessage instanceof FederationRestoreBegun) {
            System.out.println("FedManagementHandler - handling Federation Restore Begun");
            logger
                .info("FedManagementHandler - handling FederationRestoreBegun");

            // Should we pause processing of messages by the TranslationManager?
            // We need to stop making any RTI service invocations that would
            // change the
            // federation state. However, pausing the TranslationManager does
            // not allow us to receive the InitiateFederateRestore event. Maybe
            // that call should not be event based?

            // transManager.pauseMessageProcessing();
        } else if (incomingMessage instanceof InitiateFederateRestore) {
            System.out.println("FedManagementHandler - handling Initiate Federate Restore");
            logger
                .info("FedManagementHandler - handling InitiateFederateRestore");

            if (rtiAmbassador == null) {

                rtiAmbassador =
                    (OneSAFHLA13RTIAmbassador) hlaManager
                        .getOneSAFRTIAmbassador();
            }

            try {

                InitiateFederateRestore initiateFederateRestore =
                    (InitiateFederateRestore) incomingMessage;

                String requestedCheckpointName = initiateFederateRestore.getLabel();
                CheckpointManager cpManager = CheckpointManager.getInstance();
                Collection<CheckpointInformation> checkpoints = cpManager.getSavedCheckpoints();
                boolean foundCP = false;
                
                for (CheckpointInformation cpInfo : checkpoints) {
                	if (cpInfo.getSavedName().equals(requestedCheckpointName)) {
                		foundCP = true;
                		
                		System.out.println("Restore " + requestedCheckpointName +
                                " with sim time = " + Utils.convertTimeMillisWithDays(cpInfo.getSimulationTime()));
                		
                		
                		cpManager.scheduleCheckpointRestoreRequest(
                				requestedCheckpointName, 
                				cpInfo.getSimulationStartTime(),
                				cpInfo.getSimulationTime(),
                				Long.MAX_VALUE);
                		
                		// set this federate's handle to the value specified in the
                        // initiateFederateRestore() call
                        HLA13RTIHandleManager rtiHandleManager =
                            (HLA13RTIHandleManager) hlaManager.getRTIHandleManager();

                        hlaManager.getFederationData().setFederateRTIHandle(rtiHandleManager
                            .newFederateHandle(new Integer(initiateFederateRestore
                                .getFederateHandle())));

                        // wait for the simulation state to transitioned to Restored
                        // to send the federateRestoreComplete() callback
                        waitingForRestored = true;
                		break;
                	}
                }
                
                if (!foundCP) {
            		logger.error("FedManagementHandler - Failure to locate requested checkpoint file " + requestedCheckpointName);
            		// inform the RTI that this federate has not completed
                    // restoring its federate-managed state
                    rtiAmbassador.federateRestoreNotComplete();
            	}

            } catch (Exception e) {

                logger.error("FedManagementHandler - caught Exception while"
                    + " restoring checkpoint", e);

                try {

                    // inform the RTI that this federate has not completed
                    // restoring its federate-managed state
                    rtiAmbassador.federateRestoreNotComplete();
                } catch (Exception e2) {
                    // notification of RTI -- we don't care if there is an error
                }
            }
        } else if (incomingMessage instanceof FederationRestored) {
            System.out.println("FedManagementHandler - handling Federation Restored");
            logger.info("FedManagementHandler - handling FederationRestored");
        } else if (incomingMessage instanceof FederationNotRestored) {
            System.out.println("FedManagementHandler - handling Federation NOT Restored");
            logger
                .info("FedManagementHandler - handling FederationNotRestored");
        }
    }

    @Override
    public void handleMessage(AbstractReturnMessage retMessage) {

        if (retMessage instanceof CreateFederationExecReturn) {

            CreateFederationExecReturn msg =
                (CreateFederationExecReturn) retMessage;

            if (msg.getException() == null || // created successfully
                msg.getException().getCause() instanceof FederationExecutionAlreadyExists) {

                createFederationExecReturn(true);
            } else { // create failed
                logger.error("Create failed with exception: ", msg
                    .getException());
                createFederationExecReturn(false);
            }
        } else if (retMessage instanceof JoinFederationExecReturn) {

            JoinFederationExecReturn msg =
                (JoinFederationExecReturn) retMessage;

            if (msg.getException() == null || // joined successfully
                msg.getException().getCause() instanceof FederateAlreadyExecutionMember) {

                joinFederationExecReturn(msg.getFederateHandle());
            } else { // join failed
                logger
                    .error("Join failed with exception: ", msg.getException());
                joinFederationExecReturn(null);
            }
        } else if (retMessage instanceof ResignFederationExecReturn) {

            ResignFederationExecReturn msg =
                (ResignFederationExecReturn) retMessage;

            if (msg.getException() == null) { // resign successful
                if (debugEnabled) {
                    logger.debug("Federation resigned succesfully!");
                }

                resignFederationExecReturn(true);
            } else { // resign failed
                logger.error("Resign failed with exception: ", msg
                    .getException());
                resignFederationExecReturn(false);
            }
        } else if (retMessage instanceof DestroyFederationExecReturn) {

            DestroyFederationExecReturn msg =
                (DestroyFederationExecReturn) retMessage;

            if (msg.getException() == null || // destroyed successfully
                msg.getException().getCause() instanceof FederationExecutionDoesNotExist) {

                destroyFederationExecReturn(true);
            } else { // destroy failed
                logger.error("Destroy failed with exception: ", msg
                    .getException());
                destroyFederationExecReturn(false);
            }
        }
    }

    @Override
    public void interactionFired(Object proxy) {

        if (proxy instanceof CheckpointSaveFailedAlert) {

            logger
                .error("FedManagementHandler - received CheckpointSaveFailedAlert");

            try {

                // inform the RTI that this federate has not completed
                // saving its federate-managed state
                rtiAmbassador.federateSaveNotComplete();
            } catch (Exception e) {
                // notification of RTI -- we don't care if there is an error
            }
        } else if (proxy instanceof CheckpointRestoreFailedAlert) {

            logger
                .error("FedManagementHandler - recevied CheckpointRestoreFailedAlert");

            try {

                // inform the RTI that this federate has not completed
                // restoring its federate-managed state
                rtiAmbassador.federateRestoreNotComplete();
            } catch (Exception e) {
                // notification of RTI -- we don't care if there is an error
            }
        }
    }
}
