(setq path nil)

(defun cddr(a)
(cdr (cdr a)))

(defun caddr(a)
(car (cdr (cdr a))))

(defun ceil(timestamp item)
  (if (eq (big  (mktime (spaceconcat (list  (car item)  
                                            (car (cdr item)))))
                timestamp)
          1)
      (progn
        (print path)
        (setq path  (caddr item))
        (mktime (spaceconcat (list  (car item)  (car (cdr item))))))
    timestamp))

(defun compare(item timestamp)
  (if (not (eq  (size item) 19))
      timestamp
    (ceil timestamp (cddr  
                     (cddr 
                      (cddr 
                       (cddr  
                        (cddr   
                         (cddr 
                          (cddr 
                           (cddr item)))))))))))

(defun find_path(lst timestamp)
  (if  (eq lst nil)
      timestamp
    (find_path (cdr lst) 
               (compare (spacesplit (car lst))
                        timestamp))))

(defun get_list()
  (progn
    (system (print (concat (storage hadoop fs  -ls /app/dpf/quota_analyse/version_2014 )
                           (storage 1 >result ))))
    (system (print (storage cat result | grep /app/dpf/quota_analyse/version_2014/mid_result > owner )))
    (fload 'owner)))

(defun update_quota()
  (progn
    (system (print  (storage rm vertex_7)))
    (system (print (concat (storage hadoop fs  -getmerge )
                           path
                           (storage /vertex_7 . ))))
    (system (print  (storage cat vertex_7  | grep /app/dt/ups |  python space_tree.py )))
    (system (print  (storage rm vertex_78 )))))

(defun main()
  (progn
    (find_path (entersplit (get_list))  0)
    (update_quota)
    (sleep 1000)
    (main)))

(main)
