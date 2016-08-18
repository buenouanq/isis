;;; isis.el --- Isis mode, and its idiosyncratic commands.

;; Author: Stefan Agamanolis <stefan@media.mit.edu>
;; Keywords: languages, lisp, scheme, isis

;;; Commentary:

;; Adapted from Scheme mode by Bill Rozas <jinz@prep.ai.mit.edu>

;;; Code:

(defvar isis-mode-syntax-table nil "")
(if (not isis-mode-syntax-table)
    (let ((i 0))
      (setq isis-mode-syntax-table (make-syntax-table))
      (set-syntax-table isis-mode-syntax-table)

      ;; Default is atom-constituent.
      (while (< i 256)
	(modify-syntax-entry i "_   ")
	(setq i (1+ i)))

      ;; Word components.
      (setq i ?0)
      (while (<= i ?9)
	(modify-syntax-entry i "w   ")
	(setq i (1+ i)))
      (setq i ?A)
      (while (<= i ?Z)
	(modify-syntax-entry i "w   ")
	(setq i (1+ i)))
      (setq i ?a)
      (while (<= i ?z)
	(modify-syntax-entry i "w   ")
	(setq i (1+ i)))

      ;; Whitespace
      (modify-syntax-entry ?\t "    ")
      (modify-syntax-entry ?\n ">   ")
      (modify-syntax-entry ?\f "    ")
      (modify-syntax-entry ?\r "    ")
      (modify-syntax-entry ?  "    ")

      ;; These characters are delimiters but otherwise undefined.
      ;; Brackets and braces balance for editing convenience.
      (modify-syntax-entry ?[ "(]  ")
      (modify-syntax-entry ?] ")[  ")
      (modify-syntax-entry ?{ "(}  ")
      (modify-syntax-entry ?} "){  ")
      (modify-syntax-entry ?\| "  23")

      ;; Other atom delimiters
      (modify-syntax-entry ?\( "()  ")
      (modify-syntax-entry ?\) ")(  ")
      (modify-syntax-entry ?\# "<   ")
      (modify-syntax-entry ?\" "\"    ")
      (modify-syntax-entry ?' "  p")
      (modify-syntax-entry ?` "  p")

      ;; Special characters
      (modify-syntax-entry ?, "_ p")
      (modify-syntax-entry ?@ "_ p")
      (modify-syntax-entry ?; "_ p14")
      (modify-syntax-entry ?\\ "\\   ")))

(defvar isis-mode-abbrev-table nil "")
(define-abbrev-table 'isis-mode-abbrev-table ())

(defun isis-mode-variables ()
  (set-syntax-table isis-mode-syntax-table)
  (setq local-abbrev-table isis-mode-abbrev-table)
  (make-local-variable 'paragraph-start)
  (setq paragraph-start (concat "$\\|" page-delimiter))
  (make-local-variable 'paragraph-separate)
  (setq paragraph-separate paragraph-start)
  (make-local-variable 'paragraph-ignore-fill-prefix)
  (setq paragraph-ignore-fill-prefix t)
  (make-local-variable 'indent-line-function)
  (setq indent-line-function 'isis-indent-line)
  (make-local-variable 'parse-sexp-ignore-comments)
  (setq parse-sexp-ignore-comments t)
  (make-local-variable 'comment-start)
  (setq comment-start "#")
  (make-local-variable 'comment-start-skip)
  ;; Look within the line for a ; following an even number of backslashes
  ;; after either a non-backslash or the line beginning.
  (setq comment-start-skip "\\(\\(^\\|[^\\\\\n]\\)\\(\\\\\\\\\\)*\\)#+[ \t]*")
  (make-local-variable 'comment-column)
  (setq comment-column 0)
  (make-local-variable 'comment-indent-function)
  (setq comment-indent-function 'isis-comment-indent)
  (make-local-variable 'parse-sexp-ignore-comments)
  (setq parse-sexp-ignore-comments t)
  (setq mode-line-process '("" isis-mode-line-process)))

(defvar isis-mode-line-process "")

(defun isis-mode-commands (map)
  (define-key map "\t" 'isis-indent-line)
  (define-key map "\177" 'backward-delete-char-untabify)
  (define-key map "\e\C-q" 'isis-indent-sexp))

(defvar isis-mode-map nil)
(if (not isis-mode-map)
    (progn
      (setq isis-mode-map (make-sparse-keymap))
      (isis-mode-commands isis-mode-map)))

;;;###autoload
(defun isis-mode ()
  "Major mode for editing Isis code.
Editing commands are similar to those of lisp-mode.

In addition, if an inferior Isis process is running, some additional
commands will be defined, for evaluating expressions and controlling
the interpreter, and the state of the process will be displayed in the
modeline of all Isis buffers.  The names of commands that interact
with the Isis process start with \"xisis-\".  For more information
see the documentation for xisis-interaction-mode.

Commands:
Delete converts tabs to spaces as it moves back.
Blank lines separate paragraphs.  Pound signs start comments.
\\{isis-mode-map}
Entry to this mode calls the value of isis-mode-hook
if that value is non-nil."
  (interactive)
  (kill-all-local-variables)
  (isis-mode-initialize)
  (isis-mode-variables)
  (make-local-variable 'font-lock-defaults)
  (setq font-lock-defaults 
	'( isis-font-lock-keywords	     
	   nil t (("+-*/.<>=!|?$%_&~^" . "w")) beginning-of-defun
	   (font-lock-comment-start-regexp . "#")
	   (font-lock-mark-block-function . mark-defun)))
  (turn-on-font-lock)
  (column-number-mode 1)
  (run-hooks 'isis-mode-hook))

(defun isis-mode-initialize ()
  (use-local-map isis-mode-map)
  (setq major-mode 'isis-mode)
  (setq mode-name "Isis"))

(defun isis-comment-indent (&optional pos)
  (save-excursion
    (if pos (goto-char pos))
    (cond ((looking-at "###") (current-column))
	  (t
	   (skip-chars-backward " \t")
	   (max (if (bolp) 0 (1+ (current-column)))
		(let ((tem (calculate-isis-indent)))
		  (if (listp tem) (car tem) tem)))))))

(defvar isis-indent-offset nil "")
(defvar isis-indent-function 'isis-indent-function "")

(defun isis-indent-line (&optional whole-exp)
  "Indent current line as Isis code.
With argument, indent any additional lines of the same expression
rigidly along with this one."
  (interactive "P")
  (let ((indent (calculate-isis-indent)) shift-amt beg end
	(pos (- (point-max) (point))))
    (beginning-of-line)
    (setq beg (point))
    (skip-chars-forward " \t")
    (if (looking-at "[ \t]*###")
	;; Don't alter indentation of a ### comment line.
	nil
      (if (listp indent) (setq indent (car indent)))
      (setq shift-amt (- indent (current-column)))
      (if (zerop shift-amt)
	  nil
	(delete-region beg (point))
	(indent-to indent))
      ;; If initial point was within line's indentation,
      ;; position after the indentation.  Else stay at same point in text.
      (if (> (- (point-max) pos) (point))
	  (goto-char (- (point-max) pos)))
      ;; If desired, shift remaining lines of expression the same amount.
      (and whole-exp (not (zerop shift-amt))
	   (save-excursion
	     (goto-char beg)
	     (forward-sexp 1)
	     (setq end (point))
	     (goto-char beg)
	     (forward-line 1)
	     (setq beg (point))
	     (> end beg))
	   (indent-code-rigidly beg end shift-amt)))))

(defun calculate-isis-indent (&optional parse-start)
  "Return appropriate indentation for current line as isis code.
In usual case returns an integer: the column to indent to.
Can instead return a list, whose car is the column to indent to.
This means that following lines at the same level of indentation
should not necessarily be indented the same way.
The second element of the list is the buffer position
of the start of the containing expression."
  (save-excursion
    (beginning-of-line)
    (let ((indent-point (point)) state paren-depth desired-indent (retry t)
	  last-sexp containing-sexp first-sexp-list-p)
      (if parse-start
	  (goto-char parse-start)
	(beginning-of-defun))
      ;; Find outermost containing sexp
      (while (< (point) indent-point)
	(setq state (parse-partial-sexp (point) indent-point 0)))
      ;; Find innermost containing sexp
      (while (and retry (setq paren-depth (car state)) (> paren-depth 0))
	(setq retry nil)
	(setq last-sexp (nth 2 state))
	(setq containing-sexp (car (cdr state)))
	;; Position following last unclosed open.
	(goto-char (1+ containing-sexp))
	;; Is there a complete sexp since then?
	(if (and last-sexp (> last-sexp (point)))
	    ;; Yes, but is there a containing sexp after that?
	    (let ((peek (parse-partial-sexp last-sexp indent-point 0)))
	      (if (setq retry (car (cdr peek))) (setq state peek))))
	(if (not retry)
	    ;; Innermost containing sexp found
	    (progn
	      (goto-char containing-sexp)
	      (setq containing-is-bracket (looking-at "\\["))
	      (goto-char (1+ containing-sexp))
	      (if (not last-sexp)
		  ;; indent-point immediately follows open paren.
		  ;; Don't call hook.
		  (setq desired-indent (current-column))
		;; Move to first sexp after containing open paren
		(parse-partial-sexp (point) last-sexp 0 t)
		(setq first-sexp-list-p (looking-at "\\s("))
		(cond
		 ((> (save-excursion (forward-line 1) (point))
		     last-sexp)
		  ;; Last sexp is on same line as containing sexp.
		  ;; It's almost certainly a function call.
		  (parse-partial-sexp (point) last-sexp 0 t)
		  (if (and (not containing-is-bracket) 
			   (/= (point) last-sexp))
		      ;; Indent beneath first argument or, if only one sexp
		      ;; on line, indent beneath that.
		      (progn (forward-sexp 1)
			     (parse-partial-sexp (point) last-sexp 0 t)))
		  (backward-prefix-chars))
		 (t
		  ;; Indent beneath first sexp on same line as last-sexp.
		  ;; Again, it's almost certainly a function call.
		  (goto-char last-sexp)
		  (beginning-of-line)
		  (parse-partial-sexp (point) last-sexp 0 t)
		  (backward-prefix-chars)))))))
      ;; If looking at a list, don't call hook.
      (if first-sexp-list-p
	  (setq desired-indent (current-column)))
      ;; Point is at the point to indent under unless we are inside a string.
      ;; Call indentation hook except when overridden by isis-indent-offset
      ;; or if the desired indentation has already been computed.
      (cond ((car (nthcdr 3 state))
	     ;; Inside a string, don't change indentation.
	     (goto-char indent-point)
	     (skip-chars-forward " \t")
	     (setq desired-indent (current-column)))
	    ((and (integerp isis-indent-offset) containing-sexp)
	     ;; Indent by constant offset
	     (goto-char containing-sexp)
	     (setq desired-indent (+ isis-indent-offset (current-column))))
	    ((not (or desired-indent
		      (and (boundp 'isis-indent-function)
			   isis-indent-function
			   (not retry)
			   (setq desired-indent
				 (funcall isis-indent-function
					  indent-point state)))))
	     ;; Use default indentation if not computed yet
	     (setq desired-indent (current-column))))
      desired-indent)))

(defun isis-indent-function (indent-point state)
  (let ((normal-indent (current-column)))
    (save-excursion
      (goto-char (1+ (car (cdr state))))
      (re-search-forward "\\sw\\|\\s_")
      (if (/= (point) (car (cdr state)))
	  (let ((function (buffer-substring (progn (forward-char -1) (point))
					    (progn (forward-sexp 1) (point))))
		method)
	    ;; Who cares about this, really?
	    ;(if (not (string-match "\\\\\\||" function)))
	    (setq function (downcase function))
	    (setq method (get (intern-soft function) 'isis-indent-function))
	    (cond ((integerp method)
		   (isis-indent-specform method state indent-point))
		  (method
		   (funcall method state indent-point))
		  ((and (>= (length function) 3)
			(string-equal (substring function 0 3) "set"))
		   (isis-indent-defform state indent-point))))))))

(defvar isis-body-indent 2 "")

(defun isis-indent-specform (count state indent-point)
  (let ((containing-form-start (car (cdr state))) (i count)
	body-indent containing-form-column)
    ;; Move to the start of containing form, calculate indentation
    ;; to use for non-distinguished forms (> count), and move past the
    ;; function symbol.  isis-indent-function guarantees that there is at
    ;; least one word or symbol character following open paren of containing
    ;; form.
    (goto-char containing-form-start)
    (setq containing-form-column (current-column))
    (setq body-indent (+ isis-body-indent containing-form-column))
    (forward-char 1)
    (forward-sexp 1)
    ;; Now find the start of the last form.
    (parse-partial-sexp (point) indent-point 1 t)
    (while (and (< (point) indent-point)
		(condition-case nil
		    (progn
		      (setq count (1- count))
		      (forward-sexp 1)
		      (parse-partial-sexp (point) indent-point 1 t))
		  (error nil))))
    ;; Point is sitting on first character of last (or count) sexp.
    (cond ((> count 0)
	   ;; A distinguished form.  Use double isis-body-indent.
	   (list (+ containing-form-column (* 2 isis-body-indent))
		 containing-form-start))
	  ;; A non-distinguished form. Use body-indent if there are no
	  ;; distinguished forms and this is the first undistinguished
	  ;; form, or if this is the first undistinguished form and
	  ;; the preceding distinguished form has indentation at least
	  ;; as great as body-indent.
	  ((and (= count 0)
		(or (= i 0)
		    (<= body-indent normal-indent)))
	   body-indent)
	  (t
	   normal-indent))))

(defun isis-indent-defform (state indent-point)
  (goto-char (car (cdr state)))
  (forward-line 1)
  (if (> (point) (car (cdr (cdr state))))
      (progn
	(goto-char (car (cdr state)))
	(+ isis-body-indent (current-column)))))

;;; Let is different in Isis

(defun would-be-symbol (string)
  (not (string-equal (substring string 0 1) "(")))

(defun next-sexp-as-string ()
  ;; Assumes that protected by a save-excursion
  (forward-sexp 1)
  (let ((the-end (point)))
    (backward-sexp 1)
    (buffer-substring (point) the-end)))

;; This is correct but too slow.
;; The one below works almost always.
;;(defun isis-let-indent (state indent-point)
;;  (if (would-be-symbol (next-sexp-as-string))
;;      (isis-indent-specform 2 state indent-point)
;;      (isis-indent-specform 1 state indent-point)))

(defun isis-let-indent (state indent-point)
  (skip-chars-forward " \t")
  (if (looking-at "[-a-zA-Z0-9+*/?!@$%^&_:~]")
      (isis-indent-specform 2 state indent-point)
      (isis-indent-specform 1 state indent-point)))

;; (put 'begin 'isis-indent-function 0), say, causes begin to be indented
;; like defun if the first form is placed on the next line, otherwise
;; it is indented like any other form (i.e. forms line up under first).

(put 'begin 'isis-indent-function 0)
(put 'switch 'isis-indent-function 1)
(put 'while 'isis-indent-function 1)
(put 'proc 'isis-indent-function 1)
(put 'local 'isis-indent-function 'isis-let-indent)
(put 'rlocal 'isis-indent-function 'isis-let-indent)

(defun isis-indent-sexp ()
  "Indent each line of the list starting just after point."
  (interactive)
  (let ((indent-stack (list nil)) (next-depth 0) bol
	outer-loop-done inner-loop-done state this-indent)
    (save-excursion (forward-sexp 1))
    (save-excursion
      (setq outer-loop-done nil)
      (while (not outer-loop-done)
	(setq last-depth next-depth
	      innerloop-done nil)
	(while (and (not innerloop-done)
		    (not (setq outer-loop-done (eobp))))
	  (setq state (parse-partial-sexp (point) (progn (end-of-line) (point))
					  nil nil state))
	  (setq next-depth (car state))
	  (if (car (nthcdr 4 state))
	      (progn (indent-for-comment)
		     (end-of-line)
		     (setcar (nthcdr 4 state) nil)))
	  (if (car (nthcdr 3 state))
	      (progn
		(forward-line 1)
		(setcar (nthcdr 5 state) nil))
	    (setq innerloop-done t)))
	(if (setq outer-loop-done (<= next-depth 0))
	    nil
	  (while (> last-depth next-depth)
	    (setq indent-stack (cdr indent-stack)
		  last-depth (1- last-depth)))
	  (while (< last-depth next-depth)
	    (setq indent-stack (cons nil indent-stack)
		  last-depth (1+ last-depth)))
	  (forward-line 1)
	  (setq bol (point))
	  (skip-chars-forward " \t")
	  (if (or (eobp) (looking-at "[#\n]"))
	      nil
	    (if (and (car indent-stack)
		     (>= (car indent-stack) 0))
		(setq this-indent (car indent-stack))
	      (let ((val (calculate-isis-indent
			  (if (car indent-stack) (- (car indent-stack))))))
		(if (integerp val)
		    (setcar indent-stack
			    (setq this-indent val))
                  (if (cdr val)
                      (setcar indent-stack (- (car (cdr val)))))
		  (setq this-indent (car val)))))
	    (if (/= (current-column) this-indent)
		(progn (delete-region bol (point))
		       (indent-to this-indent)))))))))

(defvar isis-font-lock-keywords
  (list
   
   ;; set or bound? expressions
   
   (list (concat "(\\(set\\|bound\\?\\)\\>"
		 "[ \t]*(?"                  ;;; whitespace
		 "\\(\\sw+\\)?")             ;;; variable name (2)
	 '(1 font-lock-keyword-face)
	 '(2 font-lock-variable-name-face nil t))
   
   ;; reserved words
   ;; (make-regexp '("and" "or" "nand" "nor" "switch" "cond" "begin"
   ;;      	     "while" "else" "if" "proc" "local" "rlocal"))
  
   (list (concat "(\\("
		 "and\\|begin\\|cond\\|else\\|if\\|local\\|"
		 "n\\(and\\|or\\)\\|or\\|"
		 "proc\\|rlocal\\|switch\\|while"
		 "\\)\\>") 1 'font-lock-keyword-face)
   
   ;; character constants
   
   (list "'\\(\\sw+\\)'" 1 'font-lock-function-name-face)
   
   ;; brackets
   
   (list "\\(\\[\\|\\]\\)" 1 'font-lock-reference-face)

   ))

(setq auto-mode-alist (append '(("\\.isis$" . isis-mode)) auto-mode-alist))

(global-set-key "%" 'match-paren)
(defun match-paren (arg)
  "Go to the matching parenthesis if on parenthesis otherwise insert %."
  (interactive "p")
  (cond ((looking-at "\\s\(") (forward-list 1) (backward-char 1))
	((looking-at "\\s\)") (forward-char 1) (backward-list 1))
	(t (self-insert-command (or arg 1)))))

(provide 'isis)

;;; isis.el ends here
