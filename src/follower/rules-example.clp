(deftemplate my_metric_enabled
 (slot metric)
)

(deftemplate my_parameters
 (slot time_report)
 (slot time_tests)
 (slot time_latency)
)

(deftemplate my_metric_state
    (slot metric)
    (slot state)
)

(defrule loadFacts (declare (salience 20))
 =>
 (LoadFactsFollower)
)

; decrease time report when metric free_cpu is unstable
(defrule ch_tr_unstable (declare (salience 10))
 (my_metric_state (metric free_cpu) (state unstable))
 (my_parameters (time_report ?tr))
 =>
 (bind ?x (MetricInStateFor "free_cpu" "unstable"))
 (ChangeTimeReport (GetTimeReport "unstable" ?x ?tr))
)

; disable all metrics but battery when battery 
(defrule dm_al_dec (declare (salience 10))
 (my_metric_state (metric battery) (state decreasing))
 (my_metric_state (metric battery) (state alarming_low))
 =>
 (DisableMetrics (create$ bandwidth))
)