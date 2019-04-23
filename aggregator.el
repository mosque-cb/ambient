(seq  map_schema  (list 'id 'url 'count))
(seq  reduce_schema  (list 'id 'geqcount 'urlcount))

(defun  work (f first label)
  (if (eq (funcall f  first)  1)
      (yield (list first label))
    (if (eq label 1)
	(stop)
      (killjson first))))

(defun  filter (stream f)
  (lambda (stream f)
    (progn
      (work f  (next stream) (isstop stream) )
      (self stream f))))


(defun  select (stream f)
  (lambda (stream f)
    (progn
      (yield (list (funcall f  (next stream)) 
		   (isstop stream)))
      (self stream f))))


(defun  map (stream f)
  (lambda (stream f)
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


(defun countotherurl (first now label )
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

(defun countsameid (first now label)
  (if  (eq  (jgetstring (jgetobject first 'url))
	    (jgetstring (jgetobject now   'url)))
      (countsameurl first now (atoi (jgetstring (jgetobject  now 'count))) label)
    (countotherurl first now label)))

(defun countotherid (first now label)
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
    (yield (list first label))
    now))

(defun count (first now label)
  (if  (eq  (jgetstring (jgetobject first 'id))
	    (jgetstring (jgetobject now 'id)))
      (countsameid first now label)
    (countotherid first now label)))



(defun urlcount (schema content  obj)
  (if (eq schema  nil)
      obj
    (urlcount 
     (cdr schema)
     (cdr content)
     (jaddobject  obj  (car schema) (jcreatestring (car content))))))

 
(defun  wrapsplit (lst)
  (if (or (eq lst nil) (not (eq  (size lst)  3)))
      (list 'nothing 'nothing 'nothing)
    lst))

(defun  reduce (stream first)
  (lambda (stream first)
    (self stream
	  (count
	   first
	   (urlcount  map_schema (wrapsplit (tabsplit (strip (next stream)))) (jcreate))
	   (isstop stream)))))


(defun  reactor()
  (lambda  ()
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

(defun myconcat (lst)
  (if  (eq  lst  nil)
      '!!!
    (concat  (car lst)  '!!!  (myconcat (cdr lst)))))

(comment (prints (map  (reactor ) (lambda (lst) (tabconcat (list (strip lst) 1))))  ))

(comment (print (jgetstring (jgetobject (urlcount map_schema (tabsplit (print (tabconcat (list 'id 'url 'count )))) (jcreate) ) 'count))))

(printb
 (filter
  (select
   (reduce (reactor)
	   (urlcount map_schema 
		     (tabsplit (tabconcat (list 'idx 'urlx 'countx)))
		     (jcreate) ))
   (lambda (record) (jaddobject record
				'urlcount
				(jcreatestring  (tabconcat (varsort (split  (jgetstring (jgetobject record 'counturl)) '!!!)))))))
  (lambda  (record) (if  (big (atoi (jgetstring (jgetobject record 'geqcount))) 200) 1  0)) 
  ))










