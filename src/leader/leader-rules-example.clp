(deftemplate node_metric_state
    (slot node)
    (slot metric)
    (slot state)
)

(defrule loadFacts (declare (salience 20))
 =>
 (LoadFactsLeader)
)

; send change leader signal to followers
; and demote to follower
(defrule change_server_btl (declare (salience 10)
 (my_metric_state (metric battery) (state too_low))
 =>
 (SendChangeServer)
)