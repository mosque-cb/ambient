(setq  map_schema  (list 'id 'url 'count))
(setq  reduce_schema  (list 'id 'count 'url))

(defun  work (f n label)
  (if (eq (funcall f  n)  1)
      (yield (list n label))
    (if (eq label 1)
        (stop)
      nil)))

(defun  filter (stream f)
  (iter (stream f)
    (progn
      (work f  (next stream) (isstop stream) )
      (self stream f))))


(defun  select (stream f)
  (iter (stream f)
    (progn
      (yield (list (funcall f  (next stream)) 
                   (isstop stream)))
      (self stream f))))


(defun  map (stream f)
  (iter (stream f)
    (progn
      (yield (list (funcall f  (next stream)) 
                   (isstop stream)))
      (self stream f))))

(defun decision(first label)
  (if (eq label 1)
      (yield (list first
                   label))
    first))

(defun countsameurl (first now nowcount label )
  (progn
    (killjson  now)
    (decision (jupdateobject first 
                             'count 
                             (jcreatestring (itoa (add  (atoi (jgetstring (jgetobject first 'count)))
                                                        nowcount))))
              label)))


(defun countsameid (first now label)
  (if  (eq  (jgetstring (jgetobject first 'url))
            (jgetstring (jgetobject now   'url)))
      (countsameurl first now (atoi (jgetstring (jgetobject  now 'count))) label)
    (progn
      (yield (list first
                   label))
      now)))


(defun count (first now label)
  (if  (eq  (jgetstring (jgetobject first 'id))
            (jgetstring (jgetobject now 'id)))
      (countsameid first now label)
    (progn
      (yield  (list first
                    label))
      now)))


(defun format (schema content  obj)
  (if (eq schema  nil)
      obj
    (format 
     (cdr schema)
     (cdr content)
     (jaddobject  obj  (car schema) (jcreatestring (car content))))))

 
(defun  wrapsplit (lst)
  (if (or (eq lst nil) (not (eq  (size lst)  3)))
      (list 'nothing 'nothing 'nothing)
    lst))

(defun  reduce (stream first)
  (iter (stream first)
    (self stream
          (count
           first
           (format  map_schema (wrapsplit (tabsplit (strip (next stream)))) (jcreate))
           (isstop stream)))))


(defun  reactor()
  (iter  ()
    (progn
      (yield (list  (stdin)
                    (eofstdin)))
      (self  ))))

(defun switch (content first)
  (progn
    (killjson first)
    (print  content)))


(defun serial (schema first)
  (if (eq schema  nil)
      obj
    (cons
     (jgetstring (jgetobject first (car schema)))
     (serial (cdr schema)
             first))))


(defun wrapprint ( first)
  (switch  (tabconcat (serial reduce_schema first))
           first))

(defun  printb (stream )
  (if  (isstop stream)
      nil
    (progn
      (wrapprint  (next stream))
      (printb  stream))))


(defun  prints (stream )
  (if  (isstop stream)
      nil
    (progn
      (print  (next stream))
      (prints  stream))))


(prints (map  (reactor ) (lambda (lst) (tabconcat (list (strip lst) 1))))  )

(comment (print (jgetstring (jgetobject (format map_schema (tabsplit (print (tabconcat (list 'id 'url 'count )))) (jcreate) ) 'count))))

(comment
 (printb
  (filter
   (select
    (reduce (reactor)
            (format map_schema 
                    (tabsplit (tabconcat (list 'idx 'urlx 'countx) ) )
                    (jcreate) ))
    (lambda (record) (jupdateobject record
                                    'url
                                    (jcreatestring
                                     (concat (jgetstring (jgetobject record 'url ))  
                                             '#  
                                             (jgetstring (jgetobject record 'count )) )))))
   (lambda (record) (if  (big (atoi (jgetstring (jgetobject record 'count))) 4) 1  0)) 
   )))










