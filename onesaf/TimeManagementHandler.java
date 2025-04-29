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

//import hla.rti13.ifspec.LogicalTimeDouble;
//import hla.rti13.ifspec.LogicalTimeIntervalDouble;
import net.onesaf.core.services.data.dm.mdm.mgmt.SynchronizedTimeAdvanceNode;
import net.onesaf.core.services.data.dm.mdm.mgmt.intr.SynchronizedTimeAdvanceGrant;
import net.onesaf.core.services.data.dminit.DataModelInitializer;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.TranslationManager;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.fedmgt.FederateStatus;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.fedmgt.FederateStatus.FederateStatusEnum;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.fedmgt.FederateStatusListener;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.message.ret.AbstractReturnMessage;
import net.onesaf.core.services.sim.interopmgt.hla.common.framework.message.rti.AbstractRTIMessage;
import net.onesaf.core.services.sim.interopmgt.hla.common.handler.RTIMessageHandler;
import net.onesaf.core.services.sim.interopmgt.hla.common.handler.ReturnMessageHandler;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.HLA13Config;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.HLA13Manager;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.amb.OneSAFHLA13RTIAmbassador;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.ret.EnableTimeConstrainedReturn;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.ret.EnableTimeRegulationReturn;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.ret.TimeAdvanceRequestReturn;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.incoming.TimeAdvanceGrant;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.incoming.TimeConstrainedDisabled;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.incoming.TimeConstrainedEnabled;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.incoming.TimeRegulationDisabled;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.incoming.TimeRegulationEnabled;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.outgoing.EnableTimeConstrained;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.framework.message.rti.outgoing.EnableTimeRegulation;
import net.onesaf.core.services.sim.interopmgt.hla.hla13.handler.standard.config.TimeManagementHandlerConfig;
import net.onesaf.core.services.sim.simeng.common.DistributionStateManager;
import net.onesaf.core.services.sys.composition.ComponentManager;
import net.onesaf.core.services.sys.logger2.OneSAFLogger;
import net.onesaf.core.services.sys.odb.InteractionListener;
import net.onesaf.core.services.sys.odb.ObjectDatabase;
import org.portico.impl.hla13.types.DoubleTime;
import org.portico.impl.hla13.types.DoubleTimeInterval;

/**
 * This class provides time management functionality that is specific for use 
 * with the HLA 1.3 federations.
 *
 * @version $Revision$
 * @author $Author$
 */
public class TimeManagementHandler
    implements FederateStatusListener, RTIMessageHandler, ReturnMessageHandler {

    /**
     * Reference to the OneSAFLogger object.
     */
    protected static OneSAFLogger logger = (OneSAFLogger) OneSAFLogger.
        getLogger(TimeManagementHandler.class);

    /**
     * Reference to the ObjectDatabase.
     */
    protected ObjectDatabase odb;

    /**
     * Reference to the HLA13Manager object.
     */
    protected HLA13Manager hlaManager;

    /**
     * Reference to the TranslationManager object.
     */
    protected TranslationManager transManager;

    /**
     * Reference to the TimeManagerHandlerConfig object.
     */
    protected TimeManagementHandlerConfig timeManagementHandlerConfig;
    
    /**
     * Reference to the OneSAFHLA13RTIAmbassador object.
     */
    protected OneSAFHLA13RTIAmbassador rtiAmbassador = null;

    /**
     * Lookahead value to use on the RTI for TARs and lookahead registration
     * for this federate.
     */
    protected double lookahead = 0.0;

    /**
     * The currently known federation time received from RTI callbacks.
     */
    protected double federationTime = 0.0;

    /**
     * The last known simulation time from OOS.
     */
    protected long simulationTime = 0;

    /**
     * Reference to the SynchronizedTimeAdvanceNode that we maintain in the ODB
     * when we are constrained by the external RTI federation.
     */
    private SynchronizedTimeAdvanceNode stan = null;

    /**
     * Reference to the RequestListener object to use for getting ODB time
     * grant updates.
     */
    protected RequestListener requestListener = null;

    /**
     * Boolean indicating if we should auto-detect the RTI start time
     * (if possible).
     */
    protected boolean autoDetectTime = false;

    /**
     * Boolean indicatin if this HLA federate is time constrained.
     */
    protected boolean constrained = false;

    /**
     * Boolean indicating if this HLA federate is time regulating.
     */
    protected boolean regulating = false;

    protected boolean timeAdvanceRequestInProgress = false;

    /**
     * Value to track the latest TAR attempted so when a TAG is finally
     * received by the RTI, the last TAR will be queued for output.
     */
    protected long queuedTAR = -1;

    protected double lastTAR = 0.0;

    protected double lastTAG = 0.0;

    protected boolean initialized;

    /**
     * Reference to the singleton instance of this class.
     */
    protected static TimeManagementHandler instance = null;

    /**
     * Protected constructor.
     */
    protected TimeManagementHandler() {
    }

    /**
     * Returns a reference to the singleton instance of this class.
     *
     * @return a reference to the singleton instance of this class
     */
    public static TimeManagementHandler getInstance() {

        if (instance == null) {
            instance = new TimeManagementHandler();
        }
        return instance;
    }

    /**
     * Performs initialization, after the federation has been joined.
     */
    public void initialize() {

        if (!initialized) {
            autoDetectTime = (timeManagementHandlerConfig.startTime < 0);
    
            double startTime = autoDetectTime
                               ? 0 : timeManagementHandlerConfig.startTime;
    
            if (timeManagementHandlerConfig.timeConstrained) {
    
                // if this HLA federate is time constrained, create and send a
                // EnableTimeConstrained event to inform the RTI that we are
                // time constrained
                new EnableTimeConstrained().send();
    
                logger.info("TimeManagementHandler - enabled time constrained mode");
            }
    
            if (timeManagementHandlerConfig.timeRegulating) {
    
                // if this HLA federate is time regulating, create and send a
                // EnableTimeRegulation event to inform the RTI that we are
                // time regulating
                try {
                    DistributionStateManager distributionStateManager = 
                        DistributionStateManager.getInstance(); 

                    lookahead = timeManagementHandlerConfig.timeLookahead;

                    // Set the HLA lookahead on the DistributionManager
                    // so that the SimEngine uses the HLA config value.
                    // Convert from HLA lookahead unit of measure hours to milliseconds 
                    // (60 minutes * 60 seconds * 1000 milliseconds)
                    distributionStateManager.setLookahead(Math.round(lookahead * 3600000));
    
                    EnableTimeRegulation enableTimeRegulation
                        = new EnableTimeRegulation();
                    enableTimeRegulation.
                        setFederateTime(new DoubleTime(startTime));
                    enableTimeRegulation.
                        setLookaheadTime(new DoubleTimeInterval(lookahead));
                    enableTimeRegulation.send();
    
                    logger.info("TimeManagementHandler - enabled time regulating"
                        + " mode start = " + startTime + " lookahead = "
                        + lookahead);
                } catch (Exception e) {
    
                    logger.error("TimeManagementHandler - could not enable time"
                        + " regulation", e);
                }
            }
            
            initialized = true;
            
        } else {
            logger.debug("TimeManagementHandler is already initialized");
        }
    }

    /**
     * Currently known federation time as received from going into  enabling
     * constrained mode.  Sets the local TAG for OOS as starting at this
     * point.  This will be updated internally AFTER the start.
     *
     * @param federationTime the currently known federation time used for
     *        skipping the TAG ahead (if necessary)
     */
    public void setFederationTime(double federationTime) {
        this.federationTime = federationTime;
    }

    /**
     * Returns the last known time granted to OOS from the RTI.
     *
     * @return the last known time granted to OOS from the RTI
     */
    public double getFederationTime() {

        return federationTime;
    }
    
    /**
     * Returns the lookahead value (hours).
     * @return the lookahead.
     */
    public double getLookahead() {
    	return lookahead;
    }
    
    /**
     * Reurns the last requested OneSAF simulation time.
     * 
     * @return the last requested OneSAF simulation time
     */
    public long getSimulationTime() {

        return simulationTime;
    }

    /**
     * This method will auto detect the current RTI time if needed and kick
     * start the OOS constained time values.
     */
    public void start() {

        if (autoDetectTime) {

            // give ourselves the initial grant if we are constrained by
            // the RTI federation (otherwise we will free run until we
            // get an update from the RTI)
            if (constrained) {

                timeAdvanceGrant(federationTime);
            }
        }
    }

    /**
     * Sends a request to the RTI from this federate to advance the simulation
     * time to the specified value.
     *
     * @param requestedOneSAFSimulationTime the OneSAF simulation time, in
     * milliseconds since the epoch, for which to request the time advance
     */
    protected void timeAdvanceRequest(long requestedOneSAFSimulationTime) {

        // only send a TAR for a NEW time -- not one in the RTI's past
        long fedSimTime = getOneSAFTime(federationTime);
        simulationTime = requestedOneSAFSimulationTime;

        if ((requestedOneSAFSimulationTime < fedSimTime) &&
            (requestedOneSAFSimulationTime < (fedSimTime - (lookahead / 2)))) {

        	if(logger.isDebugEnabled()) {
	            logger.debug("TimeManagementHandler - held off on TAR because: "
	               + simulationTime + "(" + getRTITime(simulationTime) + ")" + "<"
	               + fedSimTime + "(" + federationTime + ")");
        	}

            return;
        }

        if (timeAdvanceRequestInProgress) {

        	if(logger.isDebugEnabled()) {
        		logger.debug("TimeManagementHandler - TAR in progress without TAG yet");
        	}

            if (simulationTime > queuedTAR) {

                queuedTAR = simulationTime;
                
                if(logger.isDebugEnabled()) {
	                logger.debug("TimeManagementHandler - TAR queued for simulationTime: "
	                    + simulationTime);
                }
            }

            return;
        }

        timeAdvanceRequestInProgress = true;
        queuedTAR = -1;
        lastTAR = getRTITime(simulationTime);

        Double requestTime = (getRTITime(simulationTime) + lookahead);

        if(logger.isDebugEnabled()) {
	        logger.debug("TimeManagementHandler - making TAR to RTI: federationTime: "
	            + requestTime + " simulationTime:" + simulationTime
	            + " lookahead: " + lookahead);
        }
     
        try {

            if (rtiAmbassador == null) {

                rtiAmbassador = (OneSAFHLA13RTIAmbassador) hlaManager.
                    getOneSAFRTIAmbassador();
            }

            // request that the RTI advance time to the requested time - this
            // must be done in this thread to hold up the simulation engine from
            // advancing simulation time
            rtiAmbassador.
                timeAdvanceRequest(new DoubleTime(requestTime.doubleValue()));
        } catch (Exception e2) {
          
            // notification of RTI -- we don't care if there is an error
        }
        
        // wait this thread until the timeAdvanceGrant() callback is received
        // from the RTI - at that point, the simulation engine may progress to
        // the next simulation time
        while (timeAdvanceRequestInProgress) {
            
            try {
            
                Thread.sleep(10);
            } catch (Exception e) {
            }
        }
    }
    
    /**
     * This method is called in response to a previous request for time
     * advancement, either by processing the event queue (nextEventRequest())
     * or by time stepping (timeAdvanceRequest()).  The time that is returned
     * is the current time of the federate and is based on the time requested,
     * the time of the other federates, whether any federates are regulating
     * and constrained, and what the lookahead value is.
     *
     * @param federationTime  the granted federation time
     */
    protected void timeAdvanceGrant(double federationTime) {

        if (stan == null) {
            return;
        }

        long allowSimTime = getOneSAFTime(federationTime);
        timeAdvanceRequestInProgress = false;

        logger.info("TimeManagementHandler - received TAG from RTI: "
            + " current simulationTime: " + simulationTime
            + " granted federationTime:" + federationTime
            + " granted simulationTime: " + allowSimTime);

    //    stan.setRequestedSimulationTime(allowSimTime);

        this.federationTime = lastTAG = federationTime;

        if (queuedTAR != -1) {

            logger.info("TimeManagementHandler - sending out queued TAR: "
                + queuedTAR);

            timeAdvanceRequest(queuedTAR);
        }
    }
    
    /**
     * Change callback for the mode of constrained/unconstrained setup.
     *
     * @param changeTo boolean value to represent true if constained mode is to
     *        be enabled and false otherwise.
     */
    protected void updateConstrained(boolean changeTo) {

        if (changeTo == constrained) {
            return;
        }

        if (changeTo) {

            if (autoDetectTime) {

                // adjust the simtime to RTI time to "catch up" with
                // the expected time
            	if(logger.isDebugEnabled()) {
	                logger.debug("TimeManagementHandler - advancing federation time: "
	                    + federationTime);                
            	}
            }

            if (stan != null) {
                odb.add(stan); // start constraining OOS federates
            }
        } else {
            if (stan != null) {
                odb.remove(stan); // stop constraining OOS federates
            }
        }

        constrained = changeTo;
    }

    /**
     * Change callback for the regulating mode to enable/disable
     * @param changeTo true = regulating, false = non-regulating
     */
    protected void updateRegulating(boolean changeTo) {

        if (changeTo == regulating) {
            return;
        }

        if (changeTo) {

        	if(logger.isDebugEnabled()) {
	            logger.debug("TimeManagementHandler - adding listener for"
	                + " SynchronizedTimeAdvanceGrant interactions");
        	}

            // listen to OOS grants for time going to the RTI
            odb.getInteractionManager().
                addListener(SynchronizedTimeAdvanceGrant.class, requestListener, null);
        } else {

        	if(logger.isDebugEnabled()) {
	            logger.debug("TimeManagementHandler - removing listener for"
	                + " SynchronizedTimeAdvanceGrant interactions");
        	}

            // stop listening to OOS grants for time going to the RTI
            odb.getInteractionManager()
               .removeListener(SynchronizedTimeAdvanceGrant.class, requestListener);
        }

        regulating = changeTo;
    }

    public double getLastTAR() {

        return lastTAR;
    }

    public double getLastTAG() {

        return lastTAG;
    }

    @Override
    public void federateStatusChanged(FederateStatusEnum fedStatus) {

        if (FederateStatusEnum.POST_JOIN == fedStatus) {

        	if(logger.isDebugEnabled()) {
        		logger.debug("Joined federation, now setting constrained/regulated appropriately");
        	}

            initialize();
        } else if (FederateStatusEnum.PRE_RESIGN == fedStatus) {

            updateConstrained(false);
            updateRegulating(false);
        }
    }

     /**
     * Returns the OneSAF simulation time corresponding to the specified RTI
     * simulation time.  Both RTI and OneSAF simulation time are given in 
     * milliseconds since epoch.
     *
     * @param rtiTime the RTI time in RTI units (milliseconds since the epoch)
     * @return the OneSAF simulation time, in milliseconds since the epoch,
     * corresponding to the specified RTI time
     */
    protected long getOneSAFTime(double rtiTime) {

        return (long) rtiTime;
    }

    /**
     * Returns the RTI simulation time corresponding to the specified OneSAF
     * simulation time.  Both RTI and OneSAF simulation time are given in 
     * milliseconds since epoch.
     *
     * @param oosTime the OneSAF simulation time, in milliseconds since epoch
     * @return the RTI simulation time, in milliseconds since epoch
     */
    protected double getRTITime(long oosTime) {
        
        return oosTime;
    }

    @Override
    public void handleMessage(AbstractRTIMessage rtiMessage) {

        if (rtiMessage instanceof TimeRegulationEnabled) {

            TimeRegulationEnabled timeRegulationEnabled
                = (TimeRegulationEnabled) rtiMessage;
            setFederationTime(timeRegulationEnabled.getTime());
            updateRegulating(true);
        } else if (rtiMessage instanceof TimeRegulationDisabled) {

            updateRegulating(false);
        } else if (rtiMessage instanceof TimeConstrainedEnabled) {

            TimeConstrainedEnabled timeConstrainedEnabled
                = (TimeConstrainedEnabled) rtiMessage;
            setFederationTime(timeConstrainedEnabled.getTime());
            updateConstrained(true);
        } else if (rtiMessage instanceof TimeConstrainedDisabled) {

            updateConstrained(false);
        } else if (rtiMessage instanceof TimeAdvanceGrant) {

            TimeAdvanceGrant timeAdvanceGrant = (TimeAdvanceGrant) rtiMessage;

            timeAdvanceGrant(timeAdvanceGrant.getGrantedTime());
        }
    }

    @Override
    public void handleMessage(AbstractReturnMessage returnMessage) {

        if (returnMessage.getException() != null) {

            logger.warn(returnMessage + " had following exception: ",
                returnMessage.getException());
        }

        if (returnMessage instanceof EnableTimeConstrainedReturn) {

        } else if (returnMessage instanceof EnableTimeRegulationReturn) {

        } else if (returnMessage instanceof TimeAdvanceRequestReturn) {

        }
    }

    @Override
    public void initialize(TranslationManager transManager) {

        this.transManager = transManager;

        this.hlaManager = (HLA13Manager) transManager.getHLAManager();

        // load handler config specific to this object handler
        timeManagementHandlerConfig = (TimeManagementHandlerConfig) hlaManager.getConfig().
            getHandlerConfig(TimeManagementHandlerConfig.class);

        DataModelInitializer mdm = ComponentManager.getInstance(DataModelInitializer.class);
        odb = mdm.getObjectDatabase();

        HLA13Config config = hlaManager.getConfig();

        requestListener = new RequestListener();
        lookahead = timeManagementHandlerConfig.timeLookahead;
        stan = (SynchronizedTimeAdvanceNode) odb.
            create(SynchronizedTimeAdvanceNode.class);
        stan.setIdentifier(config.name);
        stan.setRequestedSimulationTime(-1);

        // register to RTI incoming & return messages
        registerRTIMessages();
        registerReturnMessages();

        // listen for federation status changes
        FederateStatus federateStatus = FederateStatus.getInstance();
        federateStatus.addFederateStatusListener(this);
    }

    public void registerRTIMessages() {

        Set<Class <? extends AbstractRTIMessage>> rtiMsgInterests = new HashSet<>();
        rtiMsgInterests.add(TimeRegulationEnabled.class);
        rtiMsgInterests.add(TimeRegulationDisabled.class);
        rtiMsgInterests.add(TimeConstrainedEnabled.class);
        rtiMsgInterests.add(TimeConstrainedDisabled.class);
        rtiMsgInterests.add(TimeAdvanceGrant.class);
        transManager.registerRTIInterest(this, rtiMsgInterests);
    }

    public void registerReturnMessages() {

        Set<Class <? extends AbstractReturnMessage>> returnMsgInterests = new HashSet<>();
        returnMsgInterests.add(EnableTimeConstrainedReturn.class);
        returnMsgInterests.add(EnableTimeRegulationReturn.class);
        returnMsgInterests.add(TimeAdvanceRequestReturn.class);
        transManager.registerReturnInterest(this, returnMsgInterests);
    }

    /**
     * Listens for SynchronizedTimeAdvanceGrant interactions.
     */
    public class RequestListener
        implements InteractionListener {

        /**
         * Issues a time advance request to the RTI when an internal OOS 
         * simulation time grant is received.
         *
         * @param obj the SynchronizedTimeAdvanceGrant
         */
        @Override
        public void interactionFired(Object obj) {

            SynchronizedTimeAdvanceGrant grant
                = (SynchronizedTimeAdvanceGrant) obj;

            if(logger.isDebugEnabled()) {
	            logger.debug("TimeManagementHandler - received OOS "
	                + " SynchronizedTimeAdvanceGrant: time: "
	                + grant.getSimulationTime());
            }

            timeAdvanceRequest(grant.getSimulationTime());
        }
    }
}