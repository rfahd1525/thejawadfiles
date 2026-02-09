from reportlab.lib.pagesizes import letter
from reportlab.platypus import SimpleDocTemplate, Paragraph, Spacer, Table, TableStyle, PageBreak
from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
from reportlab.lib import colors
from reportlab.lib.units import inch
import os
import random

def redact(text):
    return f'<font color="black" backColor="black">{text}</font>'

def draw_header_footer(canvas, doc, classification="OFFICIAL USE ONLY (LE)"):
    canvas.saveState()
    # Header
    canvas.setFont('Helvetica-Bold', 10)
    canvas.drawCentredString(letter[0]/2, letter[1] - 0.5*inch, classification)
    canvas.drawCentredString(letter[0]/2, 0.5*inch, classification)
    
    # Page Number & Meta
    canvas.setFont('Helvetica', 9)
    canvas.drawString(inch, 0.5*inch, f"PAGE {doc.page} // CASE ID: 29-4401-J") 
    canvas.drawRightString(letter[0] - inch, 0.5*inch, "ORIGINATOR: FBI/NY FIELD OFFICE")
    
    canvas.restoreState()

def generate_sentence():
    subjects = ["Subject", "Target", "Suspect", "The Asset", "Chams"]
    verbs = ["facilitated", "coordinated", "arranged", "concealed", "executed", "transferred", "laundered"]
    objects = ["illegal assets", "underage transport", "narcotics shipments", "unregistered firearms", "shell company funds", "diplomatic pouches", "blackmail material"]
    locations = ["Little St. James", "Zorro Ranch", "Paris Apartment", "The Hamptons Estate", "Miami Port", "Teterboro FBO", "Swiss Vaults"]
    partners = ["Epstein", "Maxwell", "Prince Andrew", "Brunel", "Diddy", "Wexner", "Clinton"]
    
    s = random.choice(subjects)
    v = random.choice(verbs)
    o = random.choice(objects)
    l = random.choice(locations)
    p = random.choice(partners)
    
    return f"{s} {v} {o} via {redact(l)} in coordination with {redact(p)}. "

def generate_expanded_dossier():
    base_path = "/Users/rawadfahd/Projects/5M copy/images/"
    styles = getSampleStyleSheet()
    
    # Styles
    body_style = ParagraphStyle('BodyStyle', parent=styles['Normal'], fontSize=9, leading=11, fontName='Courier') # Smaller leading for density
    bold_body = ParagraphStyle('BoldBody', parent=body_style, fontName='Courier-Bold')
    header2 = ParagraphStyle('Header2', parent=styles['Heading2'], alignment=1, spaceAfter=6)

    # --- 1. FLIGHT LOGS (5+ Pages, Full Density) ---
    flight_logs_path = os.path.join(base_path, "flight_logs_analysis.pdf")
    doc1 = SimpleDocTemplate(flight_logs_path, pagesize=letter, topMargin=0.75*inch, bottomMargin=0.75*inch)
    story1 = []
    
    story1.append(Paragraph("JOINT INTELLIGENCE TASK FORCE - OPERATION MARINA", header2))
    story1.append(Spacer(1, 0.1*inch))
    story1.append(Paragraph("<b>Subject: Manifest Correlation for Tail N908JE, N550GP</b>", bold_body))
    story1.append(Paragraph(f"Comprehensive analysis of flight logs (2015-2023). Confirmed transport of {redact('HIGH VALUE ASSETS')} to {redact('LITTLE ST. JAMES')}.", body_style))
    story1.append(Spacer(1, 0.15*inch))

    # Log Data Loop - 5 Pages
    for page_num in range(1, 6):
        story1.append(Paragraph(f"<b>MANIFEST LOG - SECTOR {page_num}</b>", bold_body))
        log_data = [["DATE", "TAIL", "ORIGIN", "DEST", "PASSENGERS"]]
        
        # INCREASED DENSITY: 45 rows per page to ensure it hits bottom margin
        for _ in range(45):
            date = f"{random.randint(1,12)}/{random.randint(1,28)}/{random.randint(18,23)}"
            tail = random.choice(["N908JE", "N550GP", "N212PD", "VP-BVF"])
            orig = random.choice(["TEB", "PBI", "VNY", "LHR", "CDG", "STT", "EHAM"])
            dest = random.choice(["STT", "MIA", "LIT", "SAF", "NYC", "VCE", "DXB"])
            
            pax_list = [
                "J. Chams", 
                redact(random.choice(['V. CLINTON', 'A. DERSHOWITZ', 'K. SPACEY', 'L. WEXNER', 'B. GATES', 'PRINCE ANDREW', 'J. EPSTEIN', 'G. MAXWELL', 'S. COMBS'])),
                redact(f"FEMALE {random.randint(1,9)}")
            ]
            pax = ", ".join(pax_list[:random.randint(2,3)])
            
            row = [
                Paragraph(date, body_style),
                Paragraph(tail, body_style),
                Paragraph(orig, body_style),
                Paragraph(dest, body_style),
                Paragraph(pax, body_style)
            ]
            log_data.append(row)
        
        t = Table(log_data, colWidths=[0.8*inch, 0.8*inch, 0.5*inch, 0.5*inch, 4*inch])
        t.setStyle(TableStyle([
            ('GRID', (0,0), (-1,-1), 0.5, colors.black),
            ('VALIGN', (0,0), (-1,-1), 'TOP'),
            ('ROWBACKGROUNDS', (1,0), (-1,-1), [colors.white, colors.whitesmoke])
        ]))
        story1.append(t)
        story1.append(PageBreak())
        
    doc1.build(story1, onFirstPage=lambda c, d: draw_header_footer(c, d), onLaterPages=lambda c, d: draw_header_footer(c, d))

    # --- 2. FINANCIAL LEDGERS (5+ Pages, Full Density) ---
    financials_path = os.path.join(base_path, "financial_ledgers.pdf")
    doc2 = SimpleDocTemplate(financials_path, pagesize=letter)
    story2 = []
    
    story2.append(Paragraph("TREASURY DEPT. - FINCEN SPECIAL INVESTIGATION", header2))
    story2.append(Paragraph("<b>CONFIDENTIAL AUDIT: SHELL COMPANY NETWORK</b>", bold_body))
    story2.append(Spacer(1, 0.1*inch))
    
    # 5 Pages
    for page_num in range(1, 6):
        story2.append(Paragraph(f"<b>WIRE TRANSFER LOG - BATCH {random.randint(100,999)}</b>", bold_body))
        fin_data = [["TX ID", "SOURCE ENTITY", "BENEFICIARY", "AMOUNT", "NOTES"]]
        
        # INCREASED DENSITY: 45 rows per page
        for _ in range(45):
            tx_id = f"#{random.randint(10000, 99999)}"
            src = random.choice(["Epstein Trust", "Maxwell LLC", "Bad Boy Global", "Clinton Fdn", "Wexner Trust", "Deutsche Bank", "JP Morgan"])
            ben = random.choice(["Chams Global", "Blue Marble SA", "North Star Ltd", "Veritas Consult", "Hyperion Fund"]) 
            if random.random() > 0.7: ben = redact("REDACTED ACCOUNT")
            
            amt = f"${random.randint(10, 900)},{random.randint(100,999)}"
            memo = random.choice(["Consulting", "Liaison Svcs", "Logistics", "Rental Fees", "Security Detail", "Talent Acquisition", "Model Fees"])
            
            row = [
                Paragraph(tx_id, body_style),
                Paragraph(src, body_style),
                Paragraph(ben, body_style),
                Paragraph(amt, body_style),
                Paragraph(memo, body_style)
            ]
            fin_data.append(row)
            
        t2 = Table(fin_data, colWidths=[0.8*inch, 1.5*inch, 1.5*inch, 1*inch, 1.5*inch])
        t2.setStyle(TableStyle([
            ('GRID', (0,0), (-1,-1), 0.5, colors.black),
            ('ROWBACKGROUNDS', (1,0), (-1,-1), [colors.white, colors.whitesmoke])
        ]))
        story2.append(t2)
        story2.append(PageBreak())

    doc2.build(story2, onFirstPage=lambda c, d: draw_header_footer(c, d, "CONFIDENTIAL // FIN"), onLaterPages=lambda c, d: draw_header_footer(c, d, "CONFIDENTIAL // FIN"))

    # --- 3. INTELLIGENCE BRIEFING (10 Pages, Wall of Text) ---
    intel_path = os.path.join(base_path, "intelligence_briefing.pdf")
    doc3 = SimpleDocTemplate(intel_path, pagesize=letter)
    story3 = []
    
    story3.append(Paragraph("FBI COUNTERINTELLIGENCE DIVISION", header2))
    story3.append(Spacer(1, 0.2*inch))
    story3.append(Paragraph("<b>OPERATION MIRROR IMAGE: JAWAD CHAMS</b>", bold_body))
    story3.append(Spacer(1, 0.2*inch))
    
    # 10 Pages
    for i in range(10): 
        story3.append(Paragraph(f"<b>INTELLIGENCE REPORT - PAGE {i+1}</b>", bold_body))
        story3.append(Spacer(1, 0.1*inch))
        
        # Generate massive blocks of text to fill the page
        narrative = ""
        for _ in range(50): # Generate 50 sentences per page
             narrative += generate_sentence()
        
        story3.append(Paragraph(narrative, body_style))
        story3.append(Spacer(1, 0.1*inch))
        
        # Add a "Field Note" box
        note = f"<b>FIELD AGENT NOTE:</b> Surveillance at {redact('STAR ISLAND')} confirms subject met with {redact('CARTEL REPRESENTATIVES')} at 0300 hours. Photos in Exhibit {random.choice(['A','B','C'])}."
        story3.append(Paragraph(note, bold_body))
        
        story3.append(PageBreak())

    doc3.build(story3, onFirstPage=lambda c, d: draw_header_footer(c, d), onLaterPages=lambda c, d: draw_header_footer(c, d))
    print("Generated High-Density Dossier")

if __name__ == "__main__":
    generate_expanded_dossier()
