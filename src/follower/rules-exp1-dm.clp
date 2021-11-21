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

(defrule dm (declare (salience 10))
 (my_metric_enabled (metric ?x&:(neq ?x battery)))
 =>
 (DisableMetrics (create$ ?x))
)