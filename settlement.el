(defun caar (lst)
  (car (car lst)))

(defun cddr (lst)
  (cdr (cdr lst)))

(defun cadr (lst)
  (car (cdr lst)))

(defun cdar (lst)
  (cdr (car lst)))

(defun caddr (lst)
  (car (cddr lst)))

(defun printlst (n)
  (if  (eq   n  nil)
      nil
    (progn
      (print    (car  n) )
      (printlst (cdr n) ))))

(defun  evif ()
  (progn
    (seq  'expr (remove_ptr 'clink))
    (if  (geq 'val)
	(progn
	  (seq  'expr  
			    (caddr 
			     (geq 'expr)))
	  (wrapeval))
      (progn
      (seq  'expr  
		          (cadr 
			   (cddr 
			    (geq 'expr))))
	(wrapeval)))))


(defun  formstruct (lst)
  (if  (eq  lst  nil)
      nil
    (if  (atom lst)
	lst
      (cons  (formstruct (car lst))
	     (formstruct (cdr lst))))))

(defun  wrapdefun (lst )
  (progn
    (seq  'globaldefun  
		      (cons   lst
			      (geq  'globaldefun)))
    (print  'wrapdefun)))

(defun  funp(name global)
  (if  (eq  global  nil)
      0
    (if  (eq  name  (caar global))
	1
      (funp  name  (cdr  global)))))

(defun  findexpr(name global)
  (if  (eq  name  
	    (caar  global))
      (cdar  global)
    (findexpr  name  
	       (cdr  global))))

(defun  bindvars  (arg value env)
  (cons
   (bindvarshelp arg value)
   env))

(defun  bindvarshelp  (arg value )
  (if  (eq  arg  nil)
      nil
    (cons  ( cons  (car arg)  
		   (cons (car value)
			 nil))
	   (bindvarshelp  (cdr  arg) (cdr value)))))

(defun  varfind (arg env)
  (if  (eq  env  nil)
      0
    (if  (eq  arg  
	      (car  (car env)))
	(progn
	  (seq 'midx  
			  (cadr (car env)))
	  1)
      (varfind arg  
	       (cdr env)))))

(defun  varfetch (arg env)
  (if  (eq  arg  
	    (car  (car env)))
      (cadr  
       (car env))
    (varfetch arg  
	      (cdr env))))

(defun  findvar  (arg  env)
  (if  (varfind   arg    
		  (car env))
      (geq 'midx)
    (findvar  arg  
	      (cdr env))))

(defun   evargslast()
  (progn
    (seq  'expr  
		      (remove_ptr  'clink))
    (if  (eq  (geq 'expr)  
	      'printlst)
	(progn
	  (seq 'val 
			   (printlst  (geq 'args)))
	  (popjreturn))
      (if  (primitivep  (geq  'expr))
	  (progn
	    (seq 'val 
			     (primitive (geq 'expr) 
					(geq 'args)))
	    (popjreturn))
	(sapply)
	))))

(defun  sapply ()
  (progn    
    (progn  (seq  'expr  
		      (findexpr (geq 'expr) 
				(geq 'globaldefun))))
    (progn  (seq  'env 
		      (bindvars         (car (geq 'expr))    
					(geq 'args)
					(geq 'env))))
    (progn  (seq  'expr 
		      (cadr  (geq 'expr))))
    (wrapeval)))

(defun   evargscombi ()
  (progn
    (progn  (seq  'args 
		      (remove_ptr 'clink)))
    (progn  (join args  
		       (geq 'val)))
    (progn  (seq  'env 
		      (remove_ptr  'clink)))
    (progn  (seq  'expr  
		      (remove_ptr  'clink)))
    (evargs)))


(defun   evargs ()
  (progn
    (if  (eq (geq 'expr) nil)
	(evargslast)
      (progn
	(progn   (add_ptr   'clink  
			     (cdr (geq 'expr))))
	(progn   (add_ptr   'clink  
			     (geq 'env)))
	(progn   (add_ptr   'clink  
			     (geq 'args)))
	(progn   (add_ptr   'clink  
			     'evargscombi))
	(progn  (seq  'expr 
			  (car  (geq 'expr))))
	(wrapeval)))))


(defun  popjreturn ()
  (progn
    (progn  (seq  'expr 
		      (remove_ptr 'clink)))
    (funcall  (geq 'expr))
    ))

(defun   wrapeval ()
  (progn
      (if  (digitp  (geq 'expr ))
	  (progn
	    (progn  (seq  'val  
			      (geq 'expr)))
	    (popjreturn))
    (if  (eq  (geq 'expr) nil)
	(progn
	  (progn  (seq  'val  nil))
	  (popjreturn))
	(if  (charp  (geq 'expr))
	    (progn
	      (progn  (seq  'val 
				(findvar (geq 'expr)
					 (geq 'env))))
	      (popjreturn))
	  (if  (eq (car (geq 'expr))  'if)
	      (progn
		(progn   (add_ptr   'clink  
				     (geq 'expr)))
		(progn   (add_ptr   'clink  
				     'evif))
		(progn  (seq  'expr   
				  (cadr  (geq 'expr))))
		(wrapeval))
	    (if  (eq (car (geq 'expr))  'defun)
		(wrapdefun  (cdr (geq 'expr )))
	      (progn
		(progn   (add_ptr   'clink  
				     (car  (geq 'expr))))
		(progn  (seq  'expr   
				  (cdr (geq 'expr))))
		(progn  (seq  'args  
				  nil))
		(evargs)))))))))

(defun  generand  (count range)
  (if  (eq  count  0)
      nil
    (cons    (random  range)
	     (generand  (minus  count  1)
			range))))

(defun once  ( )
  (progn 
    (display )
    (seq  'env     nil)
    (seq  'zencode  nil)
    (seq  'zendata  nil)
    (seq 'midx  nil)
    (seq  'globaldefun  nil)

    (main   basicdefun )
    (main   basicapply )

    (seq  'env     nil)
    (seq  'zencode  nil)
    (seq  'zendata  nil)
    (seq 'midx  nil)
    (seq  'globaldefun  nil)
    (display)
    (print  'hell)))

(defun  autotest(num)
  (progn
    (once)
    (print (concat (quote times is ) num))
    (autotest (add num 1))))

(defun  main (lst  )
  (if  (eq  lst  nil)
      nil
    (progn
      (seq  'expr  (car  lst))
      (print   (wrapeval))
      (main (cdr lst)  ))))




	     (seq  'val  nil)
	     (seq  'expr  nil)
	     (seq  'env  nil)
	     (seq  'args  nil)
	     (seq 'midx  nil)

	     (define  primop  '(cons car cdr add minus mod random))

	     (define  globaldefun  nil)
	     (define  basicdefun   '( 
				   (defun  ptr  ( x  num )
				     (if  (eq  num  1)
					 (car x )
				       (ptr  (cdr  x )   
					     (minus  num  1))))

				   (defun   cut  ( x  num )
				     (if  (eq  num  0)
					 nil
				       (cons  (car  x ) 
					      (cut  (cdr x)   
						    (minus  num  1 ) ))))

				   (defun  getvalue  (x    count )
				     (if  (eq  count  0)
					 nil
				       (cons  (car  x)  
					      (getvalue (cdr x ) 
						   (minus  count  1)))))


				   (defun  swap  (lst half  count  a  b  i)
				     (if  (eq  i  half)
					 (cons   b   
						 (swap  (cdr lst) 
							half 
							count  
							a  
							b  
							(add  i 1)))
				       (if  (eq  i  count)
					   (cons  a   
						  (cdr lst))
					 (cons  (car lst) 
						(swap  (cdr lst) 
						       half 
						       count  
						       a  
						       b  
						       (add  i 1))))))



				   (defun insert  (  lst  half  count)
				     (if (big  half 0)
					 (if (big  (ptr lst half)  
						 (ptr  lst  count) )
					     (insert  (swap  lst  
							     half 
							     count  
							     (ptr lst half)  
							     (ptr  lst  count)  
							     1) 
						      (div half 2)  
						      half)
					   (insert   lst  
						     (div half 2)  
						     half))
				       lst))



				   (defun  inserthelper (lst count)
				     (if  (div  count 2)
					 (insert  lst   
						  (div count  2)  
						  count )
				       lst))   


				   (defun  appcons   ( new  old)
				     (if  (eq  new  nil)
					 old
				       (cons  (car new)  
					      (appcons  (cdr new) 
							old))))


				   (defun wrapinsert(lst  new  count)
				     (if  (eq  lst  nil)
					 new
				       (wrapinsert  (cdr lst)  
						    (inserthelper  (appcons  new  
									     (cons (car lst) 
										   nil))  
								   count) 
						    (add  count  1) )))


				   (defun  left (i)
				     (add  i  
					 i))
				   (defun  right (i)
				     (add  i  
					 (add i  1)))



				   (defun  popworker(lst  newlabel  label count)
				     (if (big  (ptr lst label)  (ptr  lst  newlabel) )
					 (popreal (swap  lst  
							 label 
							 newlabel  
							 (ptr  lst label) 
							 (ptr  lst newlabel) 
							 1)  
						  newlabel 
						  count)
				       lst))



				   (defun  popreal (lst  label count)
				     (if  (big    (left label) count)
					 lst
				       (if  (big    (right label) count)
					   (popworker  lst 
						       (left label) 
						       label 
						       count)
					 (if (big  (ptr lst (left label))  (ptr  lst  (right label)) )
					     (popworker  lst 
							 (right label) 
							 label 
							 count)
					   (popworker  lst 
						       (left label) 
						       label 
						       count)))))   
				   
				   (defun nlst (n count)
				     (if  (eq   n  nil)
					 count
				       (nlst (cdr n) 
					     (add  count
						 1))))


				   (defun  pophelper(lst count )
				     (wrappop   (popreal  lst  
							  1  
							  count)
						count))     

				   (defun  wrappop (lst  count)
				     (if  (eq  count 0)
					 lst
				       (cons  (car lst)
					      (pophelper  (cons  (ptr  lst  count)
								 (cdr  lst))
							  (minus  count  1)))))    

				   (defun  heapsort (lst)
				     (cut (wrappop (wrapinsert  lst  
								nil
								1)
						   (nlst lst 0))
					  (nlst lst 0)))


				   (defun  generand  (count range)
				     (if  (eq  count  0)
					 nil
				       (cons    (random  range)
						(generand  (minus  count  1)
							   range))))

				   ))


	     (define  basicapply    '( 
				    (printlst  (heapsort  (generand  10  100)))
				    ))

	     (define  basicshow    'hello)
	     (print   'initobject)


(once)
(autotest  30)
