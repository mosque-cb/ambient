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
    (setq  expr (eject clink))
    (if  val
	(progn
	  (setq  expr  
			    (caddr 
			     (whole expr)))
	  (wrapeval))
      (progn
      (setq  expr  
		          (cadr 
			   (cddr 
			    (whole expr))))
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
    (setq  globaldefun  
		      (cons   lst
			      (whole   globaldefun)))
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
	  (setq midx  
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
      (whole midx)
    (findvar  arg  
	      (cdr env))))

(defun   evargslast()
  (progn
    (setq  expr  
		      (eject  clink))
    (if  (eq  (whole expr)  
	      'printlst)
	(progn
	  (setq val 
			   (printlst  (whole args)))
	  (popjreturn))
      (if  (primitivep  (whole  expr))
	  (progn
	    (setq val 
			     (primitive (whole expr) 
					(whole args)))
	    (popjreturn))
	(sapply)
	))))

(defun  sapply ()
  (progn    
    (progn  (setq  expr  
		      (findexpr (whole expr) 
				(whole globaldefun))))
    (progn  (setq  env 
		      (bindvars         (car (whole expr))    
					(whole args)
					(whole  env))))
    (progn  (setq  expr 
		      (cadr  (whole expr))))
    (wrapeval)))

(defun   evargscombi ()
  (progn
    (progn  (setq  args 
		      (eject clink)))
    (progn  (join args  
		       (whole val)))
    (progn  (setq  env 
		      (eject  clink)))
    (progn  (setq  expr  
		      (eject  clink)))
    (evargs)))


(defun   evargs ()
  (progn
    (if  (eq expr nil)
	(evargslast)
      (progn
	(progn   (press   clink  
			     (cdr (whole expr))))
	(progn   (press   clink  
			     (whole env)))
	(progn   (press   clink  
			     (whole args)))
	(progn   (press   clink  
			     'evargscombi))
	(progn  (setq  expr 
			  (car  (whole expr))))
	(wrapeval)))))


(defun  popjreturn ()
  (progn
    (progn  (setq  expr 
		      (eject clink)))
    (funcall  (whole expr))
    ))

(defun   wrapeval ()
  (progn
      (if  (digitp  (whole expr ))
	  (progn
	    (progn  (setq  val  
			      (whole expr)))
	    (popjreturn))
    (if  (eq  (whole expr) nil)
	(progn
	  (progn  (setq  val  nil))
	  (popjreturn))
	(if  (charp  (whole expr))
	    (progn
	      (progn  (setq  val 
				(findvar (whole expr)
					 (whole  env))))
	      (popjreturn))
	  (if  (eq (car (whole expr))  'if)
	      (progn
		(progn   (press   clink  
				     (whole expr)))
		(progn   (press   clink  
				     'evif))
		(progn  (setq  expr   
				  (cadr  (whole expr))))
		(wrapeval))
	    (if  (eq (car (whole expr))  'defun)
		(wrapdefun  (cdr (whole expr )))
	      (progn
		(progn   (press   clink  
				     (car  (whole expr))))
		(progn  (setq  expr   
				  (cdr (whole expr))))
		(progn  (setq  args  
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
    (setq  env     nil)
    (setq  zencode  nil)
    (setq  zendata  nil)
    (setq midx  nil)
    (setq  globaldefun  nil)

    (main   basicdefun )
    (main   basicapply )

    (setq  env     nil)
    (setq  zencode  nil)
    (setq  zendata  nil)
    (setq midx  nil)
    (setq  globaldefun  nil)
    (display)
    (print  'hell)))

(defun  autotest(num)
  (progn
    (once)
    (print (concat (storage times is ) num))
    (autotest (add num 1))))

(defun  main (lst  )
  (if  (eq  lst  nil)
      nil
    (progn
      (setq  expr  (car  lst))
      (print   (wrapeval))
      (main (cdr lst)  ))))




	     (setq  val  nil)
	     (setq  expr  nil)
	     (setq  env  nil)
	     (setq  args  nil)
	     (setq  clink  nil)

	     (setq midx  nil)

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
