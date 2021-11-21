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

(defrule ch_tr_stable (declare (salience 10))
 (my_metric_state (metric free_cpu) (state stable))
 (my_parameters (time_report ?tr))
 =>
 (bind ?x (MetricInStateFor "free_cpu" "stable"))
 (ChangeTimeReport (GetTimeReport "stable" ?x ?tr))
)

(defrule ch_tr_unstable (declare (salience 10))
 (my_metric_state (metric free_cpu) (state unstable))
 (my_parameters (time_report ?tr))
 =>
 (bind ?x (MetricInStateFor "free_cpu" "unstable"))
 (ChangeTimeReport (GetTimeReport "unstable" ?x ?tr))
)