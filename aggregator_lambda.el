(seq  map_schema  (list 'id 'url 'count))
(seq  reduce_schema  (list 'id 'geqcount 'urlcount))
(seq  init 
       (format map_schema 
               (tabsplit (tabconcat (list 'idx 'urlx 'countx)))
               (jcreate)))



(defun  work (f first cont)
  (if (eq (funcall f  first)  1)
      (funcall  cont first)
    (killjson first)))

(defun decision(first label)
  (if (eq label 1)
      (print (list first
                   label))
    first))

(defun countsameurl (first now nowcount)
  (progn
    (killjson  now)
    (jupdateobject first 
                   'count 
                   (jcreatestring (itoa (add  (atoi (jgetstring (jgetobject first 'count)))
                                              nowcount))))))

(defun countotherurl (first now)
  (progn
    (jaddobject  now  'geqcount	   (jcreatestring
                                        (itoa (add  (atoi (jgetstring (jgetobject first 'geqcount )))
                                                    (atoi (jgetstring (jgetobject first 'count )))))))
    (jaddobject  now  'counturl	   (jcreatestring
                                    (concat (itoa  (atoi (jgetstring (jgetobject first 'count ))))
                                            '#  
                                            (jgetstring (jgetobject first 'url ))
                                            '!!!  
                                            (jgetstring (jgetobject first 'counturl)))))
    (killjson first)
    now))

(defun countsameid (first now)
  (if  (eq  (jgetstring (jgetobject first 'url))
            (jgetstring (jgetobject now   'url)))
      (countsameurl first now (atoi (jgetstring (jgetobject  now 'count))))
    (countotherurl first now)))

(defun countotherid (first now cont)
  (progn
    (jupdateobject  first  'geqcount	   (jcreatestring
                                            (itoa (add (atoi (jgetstring (jgetobject first 'geqcount )))
                                                       (atoi (jgetstring (jgetobject first 'count )))))))
    (jupdateobject  first  'counturl	   (jcreatestring
                                            (concat (itoa (atoi (jgetstring (jgetobject first 'count ))))  
                                                    '#  
                                                    (jgetstring (jgetobject first 'url ))
                                                    '!!!  
                                                    (jgetstring (jgetobject first 'counturl )))))
    (funcall cont first)
    now))

(defun count (first now cont)
  (if  (eq  (jgetstring (jgetobject first 'id))
            (jgetstring (jgetobject now 'id)))
      (countsameid first now) 
    (countotherid first now  cont)))



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

(defun  reactor(cont)
  (if  (eofstdin)
      nil
    (progn
      (funcall cont  (stdin))
      (reactor cont))))


(defun switch (content first cont)
  (progn
    (killjson first)
    (funcall cont  content)))


(defun serial (schema first)
  (if (eq schema  nil)
      obj
    (cons
     (jgetstring (jgetobject first (car schema)))
     (serial (cdr schema)
             first))))


(defun wrapprint ( first cont)
  (switch  (tabconcat (serial reduce_schema first))
           first cont))

 
(defun myconcat (lst)
  (if  (eq  lst  nil)
      '!!!
    (concat  (car lst)  '!!!  (myconcat (cdr lst)))))

(comment
 (reactor (funcall (lambda (cont f)
                     (lambda (result)
                       (funcall  cont  (funcall f  result))))
                   (lambda (result) (print result)) 
                   (lambda (lst) (tabconcat (list (strip lst) 1))))))

(comment (print (jgetstring (jgetobject (format map_schema (tabsplit (print (tabconcat (list 'id 'url 'count )))) (jcreate) ) 'count))))


(reactor (funcall (lambda (cont )
                    (lambda (first)
                      (seq init  
                            (count init
                                   (format  map_schema  (wrapsplit (tabsplit  (strip first))) (jcreate))
                                   cont))))
                  (funcall (lambda (cont f)
                             (lambda (result)
                               (funcall  cont  
                                         (funcall f  result))))
                           (funcall  (lambda (cont f)
                                       (lambda (result)
                                         (work f result cont)))
                                     (funcall (lambda (cont)
                                                (lambda (first)
                                                  (wrapprint first cont)))
                                              (lambda (x) (print x)))
                                     (lambda  (record) (if  (big (atoi (jgetstring (jgetobject record 'geqcount))) 200) 1  0)))
                           (lambda (record) (jaddobject record
                                                        'urlcount
                                                        (jcreatestring  (tabconcat (varsort (split  (jgetstring (jgetobject record 'counturl)) '!!!)))))))))


