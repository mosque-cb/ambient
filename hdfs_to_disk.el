(defun wrapno(no)
  (timetodata (minus (unixtime)
                     (mul no 24  60 60))))

(defun main(no)
  (if (eq no 60)
      nil
    (progn
      (print (concat 'file 'SPACE  no))
      (system (concat (storage mkdir  )
                      'SPACE
                      (wrapno no)))
      (system (print (concat (storage /home/map/tools/hadoop_platform/hadoop_client_wutai/hadoop/bin/hadoop  fs  -get  /app/ecom/cm/sh/mizong/lbs_semantic/)
                             (wrapno no)
                             (storage /ios* . ))))
      (system (concat (storage mv ios*  ) 'SPACE  (wrapno no)))
      (print (concat 'file 'SPACE  no 'SPACE 'done))
      (sleep 100)
      (main (add no 1)))))

(main (atoi (strip (stdin))))
