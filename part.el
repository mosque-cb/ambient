(setq  map_schema  (list 'id 'url))
(setq  reduce_schema  (list 'hash 'id 'url))

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


(defun format_schema (schema content  obj)
  (if (eq content  nil)
      obj
    (format_schema 
     (cdr schema)
     (cdr content)
     (jaddobject  obj  (car schema) (jcreatestring  (car content))))))


(defun  reactor()
  (iter  ()
         (progn
           (yield (list  (stdin)
                         (eofstdin)))
           (self  ))))

(defun  prints (stream )
  (if  (isstop stream)
      nil
    (progn
      (print  (next stream))
      (prints  stream))))


(defun switch (content first)
  (progn
    (killjson first)
    content))

(defun serial (schema first)
  (if (eq schema  nil)
      obj
    (cons
     (jgetstring (jgetobject first (car schema)))
     (serial (cdr schema)
             first))))

(defun wrapprint (schema first)
  (switch  (tabconcat (serial schema first))
           first))


(defun transform(schema json)
  (tabconcat
   (list
    (md (concat (jgetstring (jgetobject json 'id))  (jgetstring (jgetobject json 'url))))
    (wrapprint schema json))))

(defun  processoutput (content json)
  (if  (eq  content nil)
      (killjson  json)
    (transform 
     map_schema
     (format_schema map_schema content json))))


(defun  processoutput_x (content json)
  (wrapprint
   map_schema
   (format_schema reduce_schema content json)))

(comment
 (prints (map  (reactor ) (lambda (lst) (processoutput (tabsplit (strip lst))
                                                       (jcreate)))))
 )

(prints (map  (reactor ) (lambda (lst) (processoutput_x (tabsplit (strip lst))
                                                        (jcreate)))))

